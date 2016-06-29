/*******************************************************************************
!  Content:
!    Automatically Offloaded SGEMM Example Program Text
!
! LRZ:           Wed. Feb. 03 2016
!
!******************************************************************************/


/* System headers */
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdint.h>

#include "mkl.h"

int main(int argc, char **argv)
{
	float *A, *B, *C; /* Matrices */

	MKL_INT N = 2560; /* Matrix dimensions */
	MKL_INT LD = N; /* Leading dimension */
	int matrix_bytes; /* Matrix size in bytes */
	int matrix_elements; /* Matrix size in elements */

	float alpha = 1.0, beta = 1.0; /* Scaling factors */
	char transa = 'N', transb = 'N'; /* Transposition options */

	int i, j; /* Counters */

	matrix_elements = N * N;
	matrix_bytes = sizeof(float) * matrix_elements;

	/* Allocate the matrices */
	A = malloc(matrix_bytes);
	if (A == NULL) {
		printf("Could not allocate matrix A\n");
		return -1;
	}

	B = malloc(matrix_bytes);
	if (B == NULL) {
		printf("Could not allocate matrix B\n");
		return -1;
	}

	C = malloc(matrix_bytes);
	if (C == NULL) {
		printf("Could not allocate matrix C\n");
		return -1;
	}

	/* Initialize the matrices */
	for (i = 0; i < matrix_elements; i++) {
		A[i] = 1.0; B[i] = 2.0; C[i] = 0.0;
	}

	printf("Computing SGEMM on the host\n");
	sgemm(&transa, &transb, &N, &N, &N, &alpha, A, &N, B, &N,
			&beta, C, &N);

	printf("Enabling Automatic Offload\n");
	/* Alternatively, set environment variable MKL_MIC_ENABLE=1 */
	if (mkl_mic_enable() != 0)
	{
		printf("Could not enable Automatic Offload (no MIC devices?). "
				"Exiting.\n");
		return -1;
	}
	else
	{
		int ndevices = mkl_mic_get_device_count(); /* Number of MIC devices */

		printf("Automatic Offload enabled: %d MIC devices present\n\n",
				mkl_mic_get_device_count());

		printf("Computing SGEMM with automatic workdivision\n\n");
		sgemm(&transa, &transb, &N, &N, &N, &alpha, A, &N, B, &N,
				&beta, C, &N);

		for (i = 0; i < ndevices; i++)
		{
			/* Alternativelly, set environment variable
			 * MKL_MIC<i>_WORKDIVISION=1.0 */
			printf("Setting workdivision for device MIC:%02d to 1.0\n", i);
       
                      /* LRZ WORK FOR YOU 
			Run sgemm entirely on the target by changing AO workdivision.
                       AO workdivision can be changed by calling MKL support function
                       * mkl_mic_se_workdivision(), or by setting the env-variable
                       * MIC_MIC0_WORKDIVISION.
                      */

			mkl_mic_set_workdivision(MKL_TARGET_MIC, i, 1.0);

			printf("Resulting workdivision configuration:\n");
			double workdivision;
			mkl_mic_get_workdivision(MKL_TARGET_HOST, 0, &workdivision);
			printf("\tworkdivision[HOST] = %+4.2f\n", workdivision);
			for (j = 0; j < ndevices; j++)
			{
				mkl_mic_get_workdivision(MKL_TARGET_MIC, j, &workdivision);
				printf("\tworkdivision[MIC:%02d] = %+4.2f\n", j, workdivision);
			}

			printf("Computing SGEMM on device %02d\n\n", i);
			sgemm(&transa, &transb, &N, &N, &N, &alpha, A, &N, B, &N,
					&beta, C, &N);
		}
	}

	/* Free the matrix memory */
	free(A); free(B); free(C);

	printf("Done\n");

    return 0;
}
