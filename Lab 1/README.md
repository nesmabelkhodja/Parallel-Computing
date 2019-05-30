Nesma Belkhodja
Lab 1
-------------------------------------------------------------------------------------
This lab implements a method for solving a group of linear equations using
MPI.
Given a set of n equations with n unknowns (x1 to xn), the program will calculate the
values of x1 to xn within an error margin of e%.
The format of the file is:
~ line1: #unknowns
~ line2: absolute relative error
~ Initial values for each unknown
~ line 3 till end: the coefficients for each equation. Each equation on a line. On the
same line and after all the coefficients you will find the constant of the
corresponding equation.
--------------------------------------------------------------------------------------
To compile:
first:  module load mpi/openmpi-x86_64
then: mpicc -std=c99 -o gs gs.c -lm
--------------------------------------------------------------------------------------
To run:
./gsref 
