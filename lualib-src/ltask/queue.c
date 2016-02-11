#include "queue.h"
#include "simplelock.h"
#include <stdlib.h>
#include <assert.h>

#define DEFAULT_LEN 16

struct queue {
	int lock;
	int n;
	int head;
	int tail;
	void ** data;
};

struct queue *
queue_create() {
	struct queue *q = malloc(sizeof(*q));
	// malloc never failed
	assert(q);
	q->lock = 0;
	q->data = malloc(sizeof(void *)*DEFAULT_LEN);
	q->n = DEFAULT_LEN;
	q->head = 0;
	q->tail = 0;
	return q;
}

void
queue_release(struct queue *q, void (*dtor)(void *)) {
	spin_lock(q);
	if (dtor) {
		int head = q->head;
		int tail = q->tail;
		if (tail < head) {
			tail += q->n;
		}
		int i;
		for (i=head;i<tail;i++) {
			dtor(q->data[i%q->n]);
		}
	}
	free(q->data);
	free(q);
}

void
queue_push(struct queue *q, void* ud) {
	assert(ud != NULL);
	spin_lock(q);
	int tail = q->tail;
	q->data[tail] = ud;
	++tail;
	if (tail >= q->n) {
		tail = 0;
	}
	if (tail == q->head) {
		// expand
		void ** data = malloc(sizeof(void *)*q->n*2);
		assert(data);
		int i;
		int head = q->head;
		for (i=0;i<q->n;i++) {
			data[i] = q->data[head % q->n];
			++head;
		}
		free(q->data);
		q->data = data;
		q->head = 0;
		q->tail = q->n;
		q->n *= 2;
	} else {
		q->tail = tail;
	}
	spin_unlock(q);
}

void *
queue_pop(struct queue *q) {
	if (q->head == q->tail) {
		return NULL;
	}
	spin_lock(q);
	if (q->head == q->tail) {
		spin_unlock(q);
		return NULL;
	}
	void * ret = q->data[q->head];
	++q->head;
	if (q->head == q->n) {
		q->head = 0;
	}
	spin_unlock(q);
	return ret;
}

int
queue_empty(struct queue *q) {
	return q->head == q->tail;
}
