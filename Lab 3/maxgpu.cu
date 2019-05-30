//Nesma Belkhodja
#include <stdlib.h>
#include <stdio.h>
#include <cuda.h>
#include <time.h>

/**
This method finds the max in the array by splitting the array in two and 
comparing the integers. By the end of the procedure, the greatest int will be the first
element of the results array.
*/
__global__ void getmaxcu(int* memNums, int* memResults, int size) {
    unsigned int t_id = threadIdx.x;
    int numActiveThreads = blockDim.x;
    int num = (numActiveThreads * blockIdx.x) + t_id;
    if (num < size) {
        atomicMax((int*)memResults, (int)memNums[num]);
    }
    else {
        int result[1024];
        result[t_id] = memNums[num];
        int half;
        while(numActiveThreads > 32){
            half = numActiveThreads/2;

            if (t_id < half){
                int temp = result[t_id + half];
                if (temp > result[t_id]){
                    result[t_id] = temp;
                }
            }
        numActiveThreads = numActiveThreads/2;
        }
    }

}

/**
This method handles the memory and kernal call. It first copies over the memory,
makes call to kernal, copies over the results, and then frees the memory.
It returns the max int in the array.
*/
int memoryAndKernalCalls(unsigned int * numbers, unsigned int * result, int size){ 
    int * memNums;
    int * memResults;
    int intSize = sizeof(int);
    int sizeSq = size*intSize;
    int blocks = ceil(size/(double)1024);

    cudaMalloc((void **)&memNums, sizeSq);
    cudaMalloc((void **)&memResults, intSize);

    cudaMemcpy(memNums, numbers, sizeSq, cudaMemcpyHostToDevice);
    cudaMemcpy(memResults, result, intSize, cudaMemcpyHostToDevice);

    cudaDeviceSynchronize();
    getmaxcu<<<blocks, 1024>>>(memNums, memResults, size); //1024 threads per block

    cudaMemcpy(result, memResults, intSize, cudaMemcpyDeviceToHost);
    cudaSetDevice(1);
    
    cudaFree(memNums);
    cudaFree(memResults);
    cudaDeviceReset();

    int max = result[0];
    return(max);
}

/**
Main method, much like that of maxseq.c code provided, reads input, checks for
errors, populates the array with random numbers, and calls the appropriate 
methods to find the largest integer in 
the array.
*/
int main(int argc, char *argv[])
{
    double total_time;
    clock_t start, end;
    start = clock();
    unsigned int size = 0;  // The size of the array
    unsigned int i;  // loop index
    unsigned int * numbers; //pointer to the array
    unsigned int * result;
    
    if(argc !=2)
    {
       printf("usage: maxseq num\n");
       printf("num = size of the array\n");
       exit(1);
    }
   
    size = atol(argv[1]);
    result = (unsigned int *)malloc(sizeof(unsigned int));
    result[0] = 0;
    numbers = (unsigned int *)malloc(size * sizeof(unsigned int *));
    
    if( !numbers )
    {
       printf("Unable to allocate mem for an array of size %ld\n", size);
       exit(1);
    }

    srand(time(NULL)); // setting a seed for the random number generator
    // Fill-up the array with random numbers from 0 to size-1 
    for( i = 0; i < size; i++)
       numbers[i] = rand() % size;  
    
    int max = memoryAndKernalCalls(numbers, result, size);
    
    // print the result
    printf("The maximum number in the array is: %ld\n", max);

    free(numbers);

    //find end time
    end = clock(); //time count stops 
    total_time = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("\nTime taken: %f\n", total_time); //note: this is not the time used for computing results 
    //(the Linux time command was used) this was just for debugging purposes

    exit(0);
}

/*
   input: pointer to an array of long int
          number of elements in the array
   output: the maximum number of the array
*/
int getmax(unsigned int num[], unsigned int size)
{
  unsigned int i;
  unsigned int max = num[0];

  for(i = 1; i < size; i++)
    if(num[i] > max)
       max = num[i];

  return( max );

}