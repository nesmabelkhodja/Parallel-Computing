Nesma Belkhodja
Lab 2
------------------------------------------------------------------
The objective of this lab, written in OpenMP is to write parallel code to generate prime numbers
from 2 to N and test scalability, speedup, and performance.
------------------------------------------------------------------
To compile: gcc -g -Wall -fopenmp -std=c99 -o genprime genprimes.c
------------------------------------------------------------------
To run:
./genprime N t
Where:
N is a positive number bigger than 2 and less than or equal to 100,000
t is the number of threads and is a positive integer that does not exceed 100.