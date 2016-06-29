/*******************************************************************************
!  Content:
!    Simple fftw code for the LRZ Workshop 
!
! LRZ:           Wed. Feb. 03 2016
!
!******************************************************************************/


/*
 * SOLUTION: Uncomment 2 lines below
 */
#include <offload.h>
#pragma offload_attribute (push, target(mic))

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <complex.h>

#include "mkl.h"
/*
 * SOLUTION: Uncomment 1 line below
 */
#pragma offload_attribute (pop)

typedef MKL_Complex8 COMPLEX;

/* DFTI plan handle */
/*
 * SOLUTION: Uncomment 1 line below
 */
__attribute__ ( (target(mic)) )
DFTI_DESCRIPTOR_HANDLE handle = 0;

/* Pointer to input/output data */
/*
 * SOLUTION: Uncomment 1 line below
 */
__attribute__ ( (target(mic)) )
COMPLEX *x = 0;

static float moda(int K, int L, int M);
static void init(COMPLEX *x, int N, int H);
static int verify(COMPLEX *x, int N, int H);

int main(void)
{
    /* Size of 1D transform */
    int N = 1000000;

    /* Arbitrary harmonic */
    int H = -N/2;

    /* Execution status */
    MKL_LONG status = 0;

    int forward_ok = 1, backward_ok = 1;

    double time_start = 0, time_end = 0;
    double flops = 0;

    printf("Forward and backward 1D complex inplace transforms\n");

    printf("Allocate space for data on the host\n");
    x = (COMPLEX*)malloc( N * sizeof(COMPLEX) );
    if (0 == x) {
        printf("Error: memory allocation on host failed\n");
        exit(1);
    }

    printf("Preallocate memory on the target\n");
    /*
     * SOLUTION: Use offload pragma to preallocate memory for x on the target.
     *      (1) The lenght of x is N
     *      (2) Make sure the memory of x is aligned on 64-byte boundary
     *      (3) Make sure the allocated memory is not freed
     */
#pragma offload target(mic) in(x:length(N) align(64) alloc_if(1) free_if(0))
    {
    }

    printf("Create handle for 1D single-precision forward and backward transforms\n");

    /* 
     * SOLUTION: Offload the call to DftiCreateDescriptor to the target.
     *      (1) What would be the 'in' variables?
     *      (2) What would be the 'out' variables?
     */
#pragma offload target(mic) in(N) nocopy(handle) out(status)
    {
        status = DftiCreateDescriptor(&handle, DFTI_SINGLE,
            DFTI_COMPLEX, 1, (MKL_LONG)N );
        if (0 == status)
		{
			status = DftiCommitDescriptor(handle);
		}
    }

    if (status) {
        printf("Error: cannot create handle\n");
        exit(1);
    }

    /* 
     * SOLUTION: Offload the call to DftiComputeForward to the target.
     *      (1) Make sure x is an 'inout' variable, because this is in-place
     *      transform.
     *      (2) Do not allocate memory for x because it was preallocated.
     *      (3) Do not free momory of x because we will use it again for more
     *      transforms.
     *      (4) What would be the 'out' variables?
     */
    // We do not time the first offload.
#pragma offload target(mic) inout(x:length(N) alloc_if(0) free_if(0)) \
    nocopy(handle) out(status)
    {
        status = DftiComputeForward(handle, x);
    }

    printf("Initialize input for forward transform\n");
    init(x, N, H);

    printf("Offload forward FFT computation to the target\n");
	time_start = dsecnd();
    /*
     * SOLUTION: Offload the call to DftiComputeForward to the target.
     * This should be the same as the previous offload.
     */
#pragma offload target(mic) inout(x:length(N) alloc_if(0) free_if(0)) \
    nocopy(handle) out(status)
    {
        status = DftiComputeForward(handle, x);
    }
	time_end = dsecnd();

    if (status) {
        printf("Error: DftiComputeForward failed\n");
        exit(1);
    }

    printf("Verify result of forward FFT\n");
    forward_ok = verify(x, N, H);
    if (0 == forward_ok) {
	    flops    = 5 * N * log2((double)N) / (time_end - time_start);
        printf("\t Forward: size = %d, GFlops  = %.3f  \n", N, flops/1000000000);
    }

    printf("Initialize input for backward transform\n");
    init(x, N, -H);

    printf("Offload backward FFT computation to the target\n");

	time_start = dsecnd();
    /* 
     * SOLUTION: Offload the call to DftiComputeBackward to the target.
     *      (1) Make sure x is an 'inout' variable, because this is in-place
     *      transform.
     *      (2) Do not allocate memory for x because it was preallocated.
     *      (3) Do not free momory of x at this time.
     *      (4) What would be the 'out' variables?
     */
#pragma offload target(mic) inout(x:length(N) alloc_if(0) free_if(0)) \
    nocopy(handle) out(status)
    {
        status = DftiComputeBackward(handle, x);
    }
	time_end = dsecnd();

    if (status) {
        printf("Error: DftiComputeBackward failed\n");
        exit(1);
    }

    printf("Verify result of backward FFT\n");
    backward_ok = verify(x, N, H);
    if (0 == backward_ok) {
	    flops    = 5 * N * log2((double)N) / (time_end - time_start);
        printf("\t Backward: size = %d, GFlops  = %.3f  \n", N, flops/1000000000 );
    }

    printf("Destroy DFTI handle and free space on the target\n");
    /*
     * SOLUTION: Use offload pragma to deallocate memory of x on the target.
     *      (1) What would be 'in' variables?
     *      (2) Do the 'in' variables need to be copied in?
     */
#pragma offload target(mic) nocopy(x:length(N) alloc_if(0) free_if(1)) \
    nocopy(handle)
    {
        DftiFreeDescriptor(&handle);
    }

    printf("Free space on host\n");
    free(x);

    printf("TEST %s\n",0==forward_ok ? 
            "FORWARD FFT PASSED" : "FORWARD FFT FAILED");
    printf("TEST %s\n",0==backward_ok ? 
            "BACKWARD FFT PASSED" : "BACKWARD FFT FAILED");

    return 0;
}

/* Compute (K*L)%M accurately */
static float moda(int K, int L, int M)
{
    return (float)(((long long)K * L) % M);
}

/* Initialize array x[N] with harmonic H */
static void init(COMPLEX *x, int N, int H)
{
    float TWOPI = 6.2831853071795864769f, phase;
    int n;

    for (n = 0; n < N; n++)
    {
        phase  = moda(n,H,N) / N;
        ((float*)&x[n])[0] = cosf( TWOPI * phase ) / N;
        ((float*)&x[n])[1] = sinf( TWOPI * phase ) / N;
    }
}

/* Verify that x has unit peak at H */
static int verify(COMPLEX *x, int N, int H)
{
    float err, errthr, maxerr;
    int n;

    /*
     * Note, this simple error bound doesn't take into account error of
     * input data
     */
    errthr = 5.0f * logf( (float)N ) / logf(2.0f) * FLT_EPSILON;
    printf(" Verify the result, errthr = %.3g\n", errthr);

    maxerr = 0;
    for (n = 0; n < N; n++)
    {
        double re_exp = 0.0, im_exp = 0.0, re_got, im_got;

        if ((n-H)%N==0)
        {
            re_exp = 1;
        }

        re_got = ((float*)&x[n])[0];
        im_got = ((float*)&x[n])[1];
        err  = fabsf(re_got - re_exp) + fabsf(im_got - im_exp);
        if (err > maxerr) maxerr = err;
        if (!(err < errthr))
        {
            printf(" x[%i]: ",n);
            printf(" expected (%.7g,%.7g), ",re_exp,im_exp);
            printf(" got (%.7g,%.7g), ",re_got,im_got);
            printf(" err %.3g\n", err);
            printf(" Verification FAILED\n");
            return 1;
        }
    }
    printf(" Verified, maximum error was %.3g\n", maxerr);
    return 0;
}
