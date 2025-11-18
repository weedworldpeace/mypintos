
/*
  File for 'test-new-alg' task implementation.
*/

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "devices/timer.h"

int prios[5] = {22, 2, 31, 45, 3};
int64_t bursts[5] = {6, 5, 4, 7, 8};

void loopp(void *aux) {
  int64_t last_tick = timer_ticks();
  while (true) {
    if (timer_ticks() > last_tick) {
      msg("%s: %d", thread_current()->name, thread_current()->running_tick);
      last_tick = timer_ticks();
    }
  }
}

void test_new_alg(void) 
{
  thread_set_priority(PRI_MAX);

  char name[10];
  for (int i = 0; i < 5; i++) {
    snprintf(name, 10, "Proc%d\0", i);
    fcfs_thread_create(name, prios[i], loopp, bursts[i], NULL);
  }
  
  thread_set_priority(PRI_MIN);
}
