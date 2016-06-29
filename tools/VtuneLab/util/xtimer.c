/*
 *  xtimer.c -- Basic timing routines. Simple implementation based on
 *              gettimeofday().
 */

#include "xtimer.h"
#include <stdio.h>

#define USEC_PER_SEC 1000000UL

void xtimer_clear(xtimer_t *timer)
{
    timerclear(&timer->t_last);
    timerclear(&timer->t_elapsed);
}

void xtimer_start(xtimer_t *timer)
{
    gettimeofday(&timer->t_last, NULL);
}

void xtimer_pause(xtimer_t *timer)
{
    struct timeval time_stop, time_elapsed_curr;
    gettimeofday(&time_stop, NULL);
    timersub(&time_stop, &timer->t_last, &time_elapsed_curr);
    timeradd(&time_elapsed_curr, &timer->t_elapsed, &timer->t_elapsed);

    // Update the last checkpoint
    timer->t_last.tv_sec = time_stop.tv_sec;
    timer->t_last.tv_usec = time_stop.tv_usec;
}

float xtimer_elapsed_time(xtimer_t *timer)
{
    return timer->t_elapsed.tv_sec +
        timer->t_elapsed.tv_usec / (float) USEC_PER_SEC;
}

void xtimer_restart(xtimer_t *timer)
{
    xtimer_clear(timer);
    xtimer_start(timer);
}

void xtimer_stop(xtimer_t *timer, const char *descr)
{
    xtimer_pause(timer);
    if (!descr)
        descr = "";

    printf("Elapsed time (%s): %f s\n", descr, xtimer_elapsed_time(timer));
}
