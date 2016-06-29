#icc -mkl mkl_fft.c -o mkl_fft
icc -no-offload -mkl mkl_fft.c -o mkl_fft_noofload
