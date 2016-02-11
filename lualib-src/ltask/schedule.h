#ifndef LTASK_SCHEDULE_H
#define LTASK_SCHEDULE_H

typedef unsigned int channelid;
typedef unsigned int taskid;

struct schedule;

struct schedule * schedule_create(int threads);
void schedule_release(struct schedule *);
int schedule_threads(struct schedule *);

taskid schedule_opentask(struct schedule *, void *ud);
void schedule_closetask(struct schedule *, taskid t);

// grab a task from a queue, and set status to running
void * schedule_grabtask(struct schedule *, int thread);
// return a task to schedule, if the status is not blocked, set it to ready
void schedule_releasetask(struct schedule *, taskid t);

// select a channel, or set task status to blocked, and add task to the channel
channelid schedule_select(struct schedule *, taskid t, int n, channelid *);

channelid schedule_newchannel(struct schedule *);
void schedule_closechannel(struct schedule *, channelid c);
int schedule_isclosed(struct schedule *, channelid c);

// recv message from a channel
void * schedule_recv(struct schedule *, channelid c);
void schedule_send(struct schedule *, channelid c, void *msg);

#endif
