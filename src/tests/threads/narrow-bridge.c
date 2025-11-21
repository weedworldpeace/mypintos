#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "narrow-bridge.h"
#include "devices/timer.h"

int64_t bridge_tick = 0; 
struct semaphore sema_left_auto;
struct semaphore sema_right_auto;
struct semaphore sema_left_emer;
struct semaphore sema_right_emer;
int cter_left_auto = 0;
int cter_right_auto = 0;
int cter_left_emer = 0;
int cter_right_emer = 0;
enum car_direction dir_bridge;
int cter_bridge = 0;
bool norm_flag = false;

int emer_waiting_left = 0;
int emer_waiting_right = 0;
bool emer_started = false;

void narrow_bridge_init(void)
{
	sema_init(&sema_left_auto, 0);
	sema_init(&sema_right_auto, 0);
	sema_init(&sema_left_emer, 0);
	sema_init(&sema_right_emer, 0);
}

void arrive_bridge(enum car_priority prio, enum car_direction dir)
{
	if (prio == car_normal) {
		thread_yield();
		while (!emer_started && (emer_waiting_left != 0 || emer_waiting_right != 0)) {
			thread_yield();
		}
	}

	if (prio == car_emergency) {
		if (dir == dir_left) {
			emer_waiting_left++;
		} else {
			emer_waiting_right++;
		}
		thread_yield();
		if (dir == dir_left) {
			if (emer_waiting_left < emer_waiting_right) {
				thread_yield();
				emer_started = true;
			} else {
				emer_started = true;
			}
		} else {
			if (emer_waiting_left == 0) {
				emer_started = true;
			}
		}
	}

	if (!cter_bridge) {
		dir_bridge = dir;
		cter_bridge++;
		bridge_tick = timer_ticks();
	} else if (dir_bridge == dir && cter_bridge < 2) {
		cter_bridge++;
		bridge_tick = timer_ticks();
	} else if (prio == car_normal) {
		if (dir == dir_left) {
			cter_left_auto++;
			sema_down(&sema_left_auto);
		} else {
			cter_right_auto++;
			sema_down(&sema_right_auto);
		}
	} else {
		if (dir == dir_left) {
			cter_left_emer++;
			sema_down(&sema_left_emer);
		} else {
			cter_right_emer++;
			sema_down(&sema_right_emer);
		}
	}
}

void process_machine(int8_t opt) {
	switch (opt)
	{
	case 0:
		cter_bridge++;
		cter_left_emer--;
		bridge_tick = timer_ticks();
		sema_up(&sema_left_emer);
		break;
	case 1:
		cter_bridge++;
		cter_right_emer--;
		bridge_tick = timer_ticks();
		sema_up(&sema_right_emer);
		break;
	case 2:
		cter_bridge++;
		cter_left_auto--;
		bridge_tick = timer_ticks();
		sema_up(&sema_left_auto);
		break;
	case 3:
		cter_bridge++;
		cter_right_auto--;
		bridge_tick = timer_ticks();
		sema_up(&sema_right_auto);
		break;
	}
}

void exit_bridge(enum car_priority prio UNUSED, enum car_direction dir)
{
	cter_bridge--;
	if (!dir) {
	    if (cter_right_emer > 0) {
			if (!cter_bridge) {
				dir_bridge = dir_right;
				while (cter_bridge < 2 && cter_right_emer > 0) {
					process_machine(1);
				}
				if (cter_bridge < 2 && cter_right_auto > 0) {
					process_machine(3);
				}
			} else if (bridge_tick == timer_ticks() && cter_left_auto > 0) {
				process_machine(2);
			}
		} else if (cter_left_emer > 0) {
			process_machine(0);
		}  else if (cter_right_auto > 0) {
			if (!cter_bridge) {
				dir_bridge = dir_right;
				while (cter_bridge < 2 && cter_right_auto > 0) {
					process_machine(3);
				}
			}
		} else if (cter_left_auto > 0) {
			process_machine(2);
		}
	} else {
		if (cter_left_emer > 0) {
			if (!cter_bridge) {
				dir_bridge = dir_left;
				while (cter_bridge < 2 && cter_left_emer > 0) {
					process_machine(0);
				}
				if (cter_bridge < 2 && cter_left_auto > 0) {
					process_machine(2);
				}
			} else if (bridge_tick == timer_ticks() && cter_right_auto > 0) {
				process_machine(3);
			}
		} else if (cter_right_emer > 0) {
			process_machine(1);
		}  else if (cter_left_auto > 0) {
			if (!cter_bridge) {
				dir_bridge = dir_left;
				while (cter_bridge < 2 && cter_left_auto > 0) {
					process_machine(2);
				}
			}
		} else if (cter_right_auto > 0) {
			process_machine(3);
		}
	}
}
