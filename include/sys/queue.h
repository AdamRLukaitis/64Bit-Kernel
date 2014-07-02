#ifndef QUE
#define QUE


#include <sys/switchManager.h>

#define QUEUESIZE       50

typedef struct {
        task_struct* q[QUEUESIZE+1];		/* body of queue */
        int first;                      /* position of first element */
        int last;                       /* position of last element */
        int count;                      /* number of queue elements */
} queue;

task_struct * dequeue(queue *q);
int empty(queue *q);

void enqueue(queue *q, task_struct *x);

void init_queue(queue *q);

#endif