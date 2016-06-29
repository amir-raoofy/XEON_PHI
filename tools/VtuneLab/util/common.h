/*
 *  common.h -- Common declaration and definitions
 *  
 *  Copyright (C) 2015, Leibniz Supercomputing Centre
 *  Copyright (C) 2015, Vasileios Karakasis
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>

#ifdef __cplusplus
#   define BEGIN_C_DECLS__ extern "C" {
#   define END_C_DECLS__ }
#else
#   define BEGIN_C_DECLS__  // empty
#   define END_C_DECLS__    // empty
#endif

BEGIN_C_DECLS__

/**
 *  Initialize pseudo random number generators
 */
static inline void rand_seed()
{
    srand48(0);
}

END_C_DECLS__

#endif  // COMMON_H
