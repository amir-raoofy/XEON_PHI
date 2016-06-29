/*
 *  xtimer.h -- Basic timing routines
 *
 *  Copyright (C) 2015, Leibniz Supercomputing Centre
 *  Copyright (C) 2015, Vasileios Karakasis
 */

#ifndef TIMER_H
#define TIMER_H

#include <sys/time.h>
#include "common.h"

BEGIN_C_DECLS__

struct xtimer_struct {
    struct timeval t_last;      // last stopwatch stop
    struct timeval t_elapsed;   // elapsed time so far
};

typedef struct xtimer_struct xtimer_t;

void xtimer_clear(xtimer_t *timer);
void xtimer_start(xtimer_t *timer);
void xtimer_restart(xtimer_t *timer);
void xtimer_pause(xtimer_t *timer);
void xtimer_stop(xtimer_t *timer, const char *descr);
float xtimer_elapsed_time(xtimer_t *timer);

END_C_DECLS__

#endif  // TIMER_H
