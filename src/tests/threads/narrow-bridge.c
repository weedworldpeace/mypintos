
/* File for 'narrow_bridge' task implementation.  
   SPbSTU, IBKS, 2017 */

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "narrow-bridge.h"
#include "devices/timer.h"

struct semaphore sema_noleft;
int noleft_fullness = 0;
struct semaphore sema_noright;
int noright_fullness = 0;
struct semaphore sema_emleft;
int emleft_fullness = 0;
struct semaphore sema_emright;
int emright_fullness = 0;
struct lock mu;
enum car_direction bridge_direction;
int bridge_fullness = 0;
int64_t last_tick = 0; 

// Called before test. Can initialize some synchronization objects.
void narrow_bridge_init(void)
{
	sema_init(&sema_noleft, 0);
	sema_init(&sema_noright, 0);
	sema_init(&sema_emleft, 0);
	sema_init(&sema_emright, 0);
	lock_init(&mu);
}

void arrive_bridge(enum car_priority prio, enum car_direction dir)
{
	lock_acquire(&mu);
	if (!bridge_fullness) {
		bridge_direction = dir;
		bridge_fullness++;
		last_tick = timer_ticks();
		lock_release(&mu);
	} else {
		if (bridge_direction == dir && bridge_fullness < 2) {
			bridge_fullness++;
			last_tick = timer_ticks();
			lock_release(&mu);
		} else {
			if (prio == car_normal) {
				if (dir == dir_left) {
					noleft_fullness++;
					lock_release(&mu);
					sema_down(&sema_noleft);
					lock_acquire(&mu);
					lock_release(&mu);
				} else {
					noright_fullness++;
					lock_release(&mu);
					sema_down(&sema_noright);
					lock_acquire(&mu);
					lock_release(&mu);
				}
			} else {
				if (dir == dir_left) {
					emleft_fullness++;
					lock_release(&mu);
					sema_down(&sema_emleft);
					lock_acquire(&mu);
					lock_release(&mu);
				} else {
					emright_fullness++;
					lock_release(&mu);
					sema_down(&sema_emright);
					lock_acquire(&mu);
					lock_release(&mu);
				}
			}
		}
	}
}

void exit_bridge(enum car_priority prio UNUSED, enum car_direction dir)
{
	lock_acquire(&mu);
	if (dir == dir_left) {
		if (emleft_fullness > 0) {
			emleft_fullness--;
			last_tick = timer_ticks();
			sema_up(&sema_emleft);
		} else if (emright_fullness > 0) {
			if (bridge_fullness == 1) {
				bridge_fullness--;
				bridge_direction = dir_right;
				while (bridge_fullness < 2 && emright_fullness > 0) {
					bridge_fullness++;
					emright_fullness--;
					last_tick = timer_ticks();
					sema_up(&sema_emright);
				}
				if (bridge_fullness < 2 && noright_fullness > 0) {
					while (bridge_fullness < 2 && noright_fullness > 0) {
						bridge_fullness++;
						noright_fullness--;
						last_tick = timer_ticks();
						sema_up(&sema_noright);
					}
				}
			} else {
				if (last_tick == timer_ticks() && noleft_fullness > 0) {
					noleft_fullness--;
					last_tick = timer_ticks();
					sema_up(&sema_noleft);
				} else {
					bridge_fullness--;
				}
			}
		} else if (noleft_fullness > 0) {
			noleft_fullness--;
			last_tick = timer_ticks();
			sema_up(&sema_noleft);
		} else if (noright_fullness > 0) {
			if (bridge_fullness == 1) {
				bridge_fullness--;
				bridge_direction = dir_right;
				while (bridge_fullness < 2 && noright_fullness > 0) {
					bridge_fullness++;
					noright_fullness--;
					last_tick = timer_ticks();
					sema_up(&sema_noright);
				}
			} else {
				bridge_fullness--;
			}
		} else {
			bridge_fullness--;
		}
	} else {
		if (emright_fullness > 0) {
			emright_fullness--;
			last_tick = timer_ticks();
			sema_up(&sema_emright);
		} else if (emleft_fullness > 0) {
			if (bridge_fullness == 1) {
				bridge_fullness--;
				bridge_direction = dir_left;
				while (bridge_fullness < 2 && emleft_fullness > 0) {
					bridge_fullness++;
					emleft_fullness--;
					last_tick = timer_ticks();
					sema_up(&sema_emleft);
				}
				if (bridge_fullness < 2 && noleft_fullness > 0) {
					while (bridge_fullness < 2 && noleft_fullness > 0) {
						bridge_fullness++;
						noleft_fullness--;
						last_tick = timer_ticks();
						sema_up(&sema_noleft);
					}
				}
			} else {
				if (last_tick == timer_ticks() && noright_fullness > 0) {
					noright_fullness--;
					last_tick = timer_ticks();
					sema_up(&sema_noright);
				} else {
					bridge_fullness--;
				}
			}
		} else if (noright_fullness > 0) {
			noright_fullness--;
			last_tick = timer_ticks();
			sema_up(&sema_noright);
		} else if (noleft_fullness > 0) {
			if (bridge_fullness == 1) {
				bridge_fullness--;
				bridge_direction = dir_left;
				while (bridge_fullness < 2 && noleft_fullness > 0) {
					bridge_fullness++;
					noleft_fullness--;
					last_tick = timer_ticks();
					sema_up(&sema_noleft);
				}
			} else {
				bridge_fullness--;
			}
		} else {
			bridge_fullness--;
		}
	}
	lock_release(&mu);
}
