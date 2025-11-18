/*
  File for 'ticks-stats' task implementation.
*/

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "devices/timer.h"

void loop(void *aux) {
  while (true) {}
}

void test_ticks_stats(void) 
{
  char name[10];
  for (int i = 3; i < 13; i++) {
    snprintf(name, 10, "thread_%d\0", i);
    thread_create(name, PRI_DEFAULT, loop, NULL);
  }

  for (int i = 0; i < 10; i++) {
    timer_sleep(100);
    msg("%s: %d", "initial", thread_current()->running_tick);
    for (int j = 3; j < 13; j++) {
      snprintf(name, 10, "thread_%d\0", j);
      msg("%s: %d", name, thread_running_ticks(j));
    }
  }
}
