#ifndef SCHED_H
#define SCHED_H

#include <sys/types.h>


typedef struct task{
  int state; //-1  = queued, 0 = running 1 = done
  unsigned int flags; /* per process flags, defined below */
  int on_rq;
  pid_t pid;
  /* simplify accounting */
  int start_tick;
  int end_tick;
  int burst;
  int prio;
  int arr;
} task;

enum sched {
	fcfs = 1,
	nppri = 2,
	sjf = 3
};

#endif /* SCHED_H */
