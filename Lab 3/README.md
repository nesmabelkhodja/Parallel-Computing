Nesma Belkhodja
nb2253
_____________________________
The program then will allocate an array of x elements of unsigned int, fill it with
random numbers from 0 to x and then call a function getmax that returns the
maximum number of the array.

The sequential version (provided) is called maxseq.c -- the CUDA version is maxgpu.cu

To compile:
gcc -std=c99 -Wall -o maxseq maxseq.c
To execute:
./maxseq x

To compile:
nvcc maxgpu.cu
To execute:
./a.out x

----in both cases x is the length of the array-----