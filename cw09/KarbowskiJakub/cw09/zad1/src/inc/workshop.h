#ifndef JK_09_01_WORKSHOP_H
#define JK_09_01_WORKSHOP_H

#include <stdbool.h>

#define N_ELF 10
#define N_REINDEER 9

void workshop_lock();

void workshop_unlock();

void workshop_wait();

void workshop_stop();

bool workshop_is_stopped();

int workshop_get_reindeers();

void workshop_reindeer_return();

void workshop_reindeer_leave();

void workshop_give_reindeer_vacation_permit();

void workshop_get_reindeer_vacation_permit();

int workshop_get_reindeer_vacation_permits();

#endif
