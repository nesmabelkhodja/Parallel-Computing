#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>
#include <stdio.h>

/***** Globals ******/
float **a; /* The coefficients */
float *x;  /* The unknowns */
float *b;  /* The constants */
float *current;
float err; /* The absolute relative error */
int num = 0;  /* number of unknowns */


/****** Function declarations */
void check_matrix(); /* Check whether the matrix will converge */
void get_input();  /* Read input from file */

/********************************/

/* Function definitions: functions are ordered alphabetically ****/
/*****************************************************************/

/* 
   Conditions for convergence (diagonal dominance):
   1. diagonal element >= sum of all other elements of the row
   2. At least one diagonal element > sum of all other elements of the row
 */
void check_matrix()
{
  int bigger = 0; /* Set to 1 if at least one diag element > sum  */
  int i, j;
  float sum = 0;
  float aii = 0;
  
  for(i = 0; i < num; i++)
  {
    sum = 0;
    aii = fabs(a[i][i]);
    
    for(j = 0; j < num; j++)
       if( j != i)
   sum += fabs(a[i][j]);
       
    if( aii < sum)
    {
      printf("The matrix will not converge\n");
      exit(1);
    }
    
    if(aii > sum)
      bigger++;
    
  }
  
  if( !bigger )
  {
     printf("The matrix will not converge\n");
     exit(1);
  }
}

/******************************************************/
/* Read input from file */
void get_input(char filename[])
{
  FILE * fp;
  int i;
  int j;  
 
  fp = fopen(filename, "r");
  if(!fp)
  {
    printf("Cannot open file %s\n", filename);
    exit(1);
  }

 fscanf(fp,"%d ",&num);
 fscanf(fp,"%f ",&err);

 /* Now, time to allocate the matrices and vectors */
 a = (float**)malloc(num * sizeof(float*));
 if( !a)
  {
  printf("Cannot allocate a!\n");
  exit(1);
  }


 for(i = 0; i < num; i++) 
  {
    a[i] = (float *)malloc(num * sizeof(float)); 
    if( !a[i])
    {
    printf("Cannot allocate a[%d]!\n",i);
    exit(1);
    }
  }
 
 x = (float *) malloc(num * sizeof(float));
 if( !x)
  {
  printf("Cannot allocate x!\n");
  exit(1);
  }

 current = (float *) malloc(num * sizeof(float));
 if( !current)
  {
  printf("Cannot allocate current!\n");
  exit(1);
  }

 b = (float *) malloc(num * sizeof(float));
 if( !b)
  {
  printf("Cannot allocate b!\n");
  exit(1);
  }

 /* Now .. Filling the blanks */ 

 /* The initial values of Xs */
 for(i = 0; i < num; i++)
  fscanf(fp,"%f ", &x[i]);
 
 for(i = 0; i < num; i++)
 {
   for(j = 0; j < num; j++)
     fscanf(fp,"%f ",&a[i][j]);
   
   /* reading the b element */
   fscanf(fp,"%f ",&b[i]);
 }
 
 fclose(fp); 

}


/************************************************************/

int main(int argc, char *argv[])
{
  int i;
  int nit = 0; /* number of iterations */
  int j;
  
  if( argc != 2)
  {
    printf("Usage: gsref filename\n");
    exit(1);
  }
  
 /* Read the input file and fill the global data structure above */ 
  get_input(argv[1]);
 
  /* Check for convergence condition
  This function will exit the program if the coffeicient will never converge to the needed absolute error. This is not expected to happen for this programming assignment.
*/
  check_matrix();

  int m;
  int n;
  float *local_new_x;
  float *sum_new_x;
  int flag = 0;  
  int local_flag;
  sum_new_x = (float *) malloc(num * sizeof(float));
  local_new_x = (float *) calloc(num, sizeof(float));
  
  int first_i;
  int last_i;
  int comm_sz;
  int my_rank;
  int count;
  int temp;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  double time0;
  double time1;
  double start;
  double finish;
  double overhead = 0;
  double sol = 0;

  int q = num / comm_sz;
  int mod = num % comm_sz;
  if (my_rank < mod)
  {
    count = q + 1;
    first_i = my_rank*count;
  }
  else
  {
    count = q;
    first_i = my_rank*count + mod;
  }
  
  last_i = first_i + count;

  //flag keeps track of when the process is done
  while (flag == 0)  
  {
    nit += 1;
    local_flag = 1;

    MPI_Barrier(MPI_COMM_WORLD);

    //starting time
    time0 = MPI_Wtime();

    for (i = first_i; i < last_i; i++) 
    {
      printf("Process %d now updates index %d\n", my_rank, i);
      local_new_x[i] = b[i] + a[i][i]*x[i];
      
      for (j = 0; j < num; j++){
        local_new_x[i] -= a[i][j]*x[j];
      }

      local_new_x[i] /= a[i][i];

      temp = (local_new_x[i] - x[i]) / local_new_x[i];

      if (temp > err){
        local_flag = 0;
      }
    } 

    time1 = MPI_Wtime();
    sol += time1 - time0;

    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();

    MPI_Allreduce(local_new_x, sum_new_x, num, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(&local_flag, &flag, 1, MPI_INT, MPI_PROD, MPI_COMM_WORLD);
    
    finish = MPI_Wtime();
    overhead += finish - start; 
    
    for (i = 0; i < num; i++){
      x[i] = sum_new_x[i];
    }
    
    /* Writing to the stdout */
    if (flag == 1)
    {
      for(i = 0; i < num; i++){
        printf("%f\n", x[i]);
      }
 
    printf("total number of iterations: %d\n", nit);  
    }
    
  } 
  
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();

  exit(0);

}