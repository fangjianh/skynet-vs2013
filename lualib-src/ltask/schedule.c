#include "schedule.h"
#include "queue.h"
#include "handlemap.h"
#include "simplelock.h"
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

struct schedule {
	int threads;
	int current;
	struct queue **q;	// the ready task queue for workers
	struct handlemap * channel;	// struct channel *
	struct handlemap * task;	// struct task *
};

struct channel {
	struct queue *mq;	// message (void *) queue
	struct queue *reader;	// task
};

#define TASK_RUNNING 0
#define TASK_READY 1
#define TASK_BLOCKED 2
#define TASK_DEAD 3	// for debug
#define TASK_INIT 4	// for debug

struct task {
	void *ud;
	int status;
	int thread;
	taskid id;
};

struct schedule *
schedule_create(int threads) {
	struct schedule *s = malloc(sizeof(*s));
	s->threads = threads;
	s->q = malloc(threads * sizeof(struct queue *));
	int i;
	for (i=0;i<threads;i++) {
		s->q[i] = queue_create();
	}
	s->channel = handlemap_init();
	s->task = handlemap_init();
	s->current = 0;
	return s;
}

void
schedule_release(struct schedule *s) {
	if (s == NULL)
		return;
	// todo : clear
}

int
schedule_threads(struct schedule *s) {
	return s->threads;
}

static void
release_task(struct schedule *s, taskid id) {
	struct task * t = handlemap_release(s->task, id);
	if (t) {
		t->status = TASK_DEAD;
		free(t);
	}
}

static void
commit_task(struct schedule *s, struct task *t, int status) {
	if (atom_cas_long(&t->status, status, TASK_READY)) {
		queue_push(s->q[t->thread], (void *)(uintptr_t)(t->id));
	}
}

taskid
schedule_opentask(struct schedule * s, void *ud) {
	struct task * t = malloc(sizeof(*t));
	int c = s->current;
	int next = c+1;
	if (next >= s->threads)
		next = 0;
	s->current = next;
	t->thread = c;
	t->status = TASK_INIT;
	t->ud = ud;
	t->id = handlemap_new(s->task, t);
	commit_task(s, t, TASK_INIT);
	return t->id;
}

void
schedule_closetask(struct schedule *s, taskid t) {
	release_task(s,t);
}

void *
schedule_grabtask(struct schedule *s, int thread) {
	for (;;) {
		taskid id = (taskid)(uintptr_t)queue_pop(s->q[thread]);
		if (id == 0) {
			int i;
			for (i=1;i<s->threads;i++) {
				int t = (thread+i) % s->threads;
				id = (taskid)(uintptr_t)queue_pop(s->q[t]);
				if (id)
					break;
			}
			if (id == 0)
				return 0;
		}
		struct task * t = handlemap_grab(s->task, id);
		if (t) {
			void * ud = t->ud;
			assert(t->status != TASK_RUNNING);
			t->status = TASK_RUNNING;
			t->thread = thread;
			release_task(s, id);
			return ud;
		}
	}
}

void
schedule_releasetask(struct schedule *s, taskid id) {
	struct task * t = handlemap_grab(s->task, id);
	if (t) {
		assert(t->status != TASK_READY);
		if (t->status == TASK_RUNNING) {
			commit_task(s, t, TASK_RUNNING);
		}
		release_task(s, id);
	}
}

static void
release_channel(struct schedule *s, channelid id) {
	struct channel *c = handlemap_release(s->channel, id);
	if (c) {
		queue_release(c->mq, NULL); // todo: release message
		queue_release(c->reader, NULL);
		free(c);
	}
}

channelid
schedule_select(struct schedule *s, taskid id, int n, channelid *channels) {
	struct task * t = handlemap_grab(s->task, id);
	if (t) {
		int i;
		for (i=0;i<n;i++) {
			struct channel * c = handlemap_grab(s->channel, channels[i]);
			if (c) {
				if (!queue_empty(c->mq)) {
					release_channel(s, channels[i]);
					release_task(s, id);
					return channels[i];
				}
				release_channel(s, channels[i]);
			}
		}
		t->status = TASK_BLOCKED;
		// add id to all channels' reader
		for (i=0;i<n;i++) {
			struct channel * c = handlemap_grab(s->channel, channels[i]);
			if (c) {
				queue_push(c->reader, (void*)(uintptr_t)id);
				release_channel(s, channels[i]);
			}
		}
		release_task(s, id);
	} 
	return 0;
}

channelid
schedule_newchannel(struct schedule *s) {
	struct channel *c = malloc(sizeof(*c));
	c->mq = queue_create();
	c->reader = queue_create();
	channelid id = handlemap_new(s->channel, c);
	return id;
}

void
schedule_closechannel(struct schedule *s, channelid id) {
	release_channel(s,id);
}

int
schedule_isclosed(struct schedule *s, channelid id) {
	struct channel *c = handlemap_grab(s->channel, id);
	if (c) {
		release_channel(s,id);
		return 0;
	}
	return 1;
}

void *
schedule_recv(struct schedule *s, channelid id) {
	struct channel *c = handlemap_grab(s->channel, id);
	if (c) {
		void *msg = queue_pop(c->mq);
		release_channel(s,id);
		return msg;
	}
	return NULL;
}

void
schedule_send(struct schedule *s, channelid id, void *msg) {
	struct channel *c = handlemap_grab(s->channel, id);
	if (c) {
		queue_push(c->mq, msg);
		for (;;) {
			// wake up blocked tasks
			uintptr_t id = (uintptr_t)queue_pop(c->reader);
			if (id == 0)
				break;
			struct task * t = handlemap_grab(s->task, id);
			if (t) {
				if (t->status == TASK_BLOCKED) {
					commit_task(s, t, TASK_BLOCKED);
				}
				release_task(s, id);
			}
		}
		release_channel(s,id);
	}
}
