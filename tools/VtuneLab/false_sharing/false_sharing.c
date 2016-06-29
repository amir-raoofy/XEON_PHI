/*
 * false-sharing.c -- A set of microbenchmarks touching different architectural
 *                    characteristics (cache misses, false sharing, div usage)
 *
 * Copyright (C) 2015, Leibniz Supercomputing Centre
 * Copyright (C) 2015, Vasileios Karakasis
 */ 
#include <malloc.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#include "xtimer.h"

#define N_PER_THREAD    2*1024
#define NR_ITER         100000
#define PROCS_MAX       32

double *A, *B;
double C[PROCS_MAX];
size_t NrThreads;
size_t NrElems;

void set_cpu_affinity(int cpu)
{
    cpu_set_t cpus;
    CPU_ZERO(&cpus);
    CPU_SET(cpu, &cpus);
    sched_setaffinity(syscall(SYS_gettid), sizeof(cpus), &cpus);
}

void init()
{
    for (size_t i = 0; i < NrElems; ++i) {
        A[i] = 1;
        B[i] = 1;
    }
}

void *compute_naive(void *args)
{
    int tid = (long) args;
    set_cpu_affinity(tid);

    for (int i = 0; i < NR_ITER; ++i) {
        for (int j = tid; j < NrElems; j += NrThreads) {
            A[j] = A[j]*B[j];
            C[tid] += A[j] / (tid+1);
        }
    }

    return 0;
}

void *compute_nomiss(void *args)
{
    int tid = (long) args;
    set_cpu_affinity(tid);

    int chunk_size = N_PER_THREAD;
    for (int i = 0; i < NR_ITER; ++i) {
        for (int j = tid*chunk_size; j < (tid+1)*chunk_size; ++j) {
            A[j] = A[j]*B[j];
            C[tid] += A[j] / (tid+1);
        }
    }

    return 0;
}

void *compute_nofalse(void *args)
{
    int tid = (long) args;
    set_cpu_affinity(tid);

    int chunk_size = N_PER_THREAD;
    double l_sum = 0.0;
    for (int i = 0; i < NR_ITER; ++i) {
        for (int j = tid*chunk_size; j < (tid+1)*chunk_size; ++j) {
            A[j] = A[j]*B[j];
            l_sum += A[j] / (tid+1);
        }
    }

    C[tid] = l_sum;
    return 0;
}

void *compute_nodiv(void *args)
{
    int tid = (long) args;
    set_cpu_affinity(tid);

    int chunk_size = N_PER_THREAD;
    double l_sum = 0.0;
    double alpha = 1. / (tid+1);
    for (int i = 0; i < NR_ITER; ++i) {
        for (int j = tid*chunk_size; j < (tid+1)*chunk_size; ++j) {
            A[j] = A[j]*B[j];
            l_sum += alpha*A[j];
        }
    }

    C[tid] = l_sum;
    return 0;
}

int main()
{
    pthread_t threads[PROCS_MAX];
    char *threadenv = getenv("NUM_THREADS");

    NrThreads = (threadenv) ? atoi(threadenv) : PROCS_MAX;
    if (!NrThreads) {
        fprintf(stderr, "[Error] Invalid thread number; must be an integer\n");
        exit(1);
    } else if (NrThreads > PROCS_MAX) {
        NrThreads = PROCS_MAX;
        fprintf(stderr,
                "[Warning] Setting thead number to maximum number: %d\n",
                PROCS_MAX);
    }

    NrElems = NrThreads*N_PER_THREAD;
    A = memalign(64, NrElems*sizeof(*A));
    B = memalign(64, NrElems*sizeof(*B));
    init();

    xtimer_t tm;
    xtimer_clear(&tm);
    xtimer_start(&tm);
    // Change the function name to the desired version
    void * (*fn)(void *) = compute_naive;

    for (size_t i = 0; i < NrThreads-1; ++i)
        if (pthread_create(&threads[i], NULL, fn, (void *) i) < 0) {
            fprintf(stderr, "Failed to create thread\n");
            exit(1);
        }

    fn((void *) (NrThreads-1));
    for (size_t i = 0; i < NrThreads-1; ++i)
        if (pthread_join(threads[i], NULL) < 0) {
            fprintf(stderr, "Failed to join thread\n");
            exit(1);
        }
    
    double sum = 0;
    for (size_t i = 0; i < NrThreads; ++i)
        sum += C[i];

    xtimer_stop(&tm, "compute");
    printf("Result: %e\n", sum);
    free(A);
    free(B);
    return 0;
}
