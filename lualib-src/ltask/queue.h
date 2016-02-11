#ifndef LTASK_QUEUE_H
#define LTASK_QUEUE_H

struct queue;

struct queue * queue_create();
void queue_release(struct queue *, void (*dtor)(void *));
void queue_push(struct queue *, void * ud);
void * queue_pop(struct queue *);
int queue_empty(struct queue *);

#endif
