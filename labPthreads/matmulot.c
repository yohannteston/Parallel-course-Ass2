#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

typedef struct{
	int i;
	int j;
}coords;

double **A,**B,**C;
int n, block_size;

void* matmul(void* arg){
	int i,j,k;
	coords c = *((coords*)arg);
	// Multiply C=A*B		  
	for(i=c.i*block_size;i<c.i * block_size + block_size;i++)
		//optimization: j and k loops have been swapped
		for (k=0;k<n;k++)
			for (j=c.i*block_size;j<c.j * block_size + block_size;j++)
				C[i][j]+=A[i][k]*B[k][j];
	pthread_exit(NULL);
}


void print(double **e){
	int i,j,k;
	for(i=0;i<n;i++){
		for(j=0;j<n;j++)
			printf(" %f ",e[i][j]);
		printf("\n");	
	}
}

int main(int argc, char *argv[]) {
	int i,j,k,time,t,st;
	pthread_t* threads;
	coords* threads_coords;

	n = atoi(argv[1]);
	t = atoi(argv[2]);

	st = sqrt(t);
	if(t != st*st)
		exit(1);

	if(!(st % n))
		exit(1);
	block_size = n/st;
	// Allocate threads id
	threads = (pthread_t*)malloc(t*sizeof(pthread_t));
	threads_coords = (coords*)malloc(t*sizeof(coords));		 
	//Allocate and fill matrices
	A = (double **)malloc(n*sizeof(double *));
	B = (double **)malloc(n*sizeof(double *));
	C = (double **)malloc(n*sizeof(double *));
	for(i=0;i<n;i++){
		A[i] = (double *)malloc(n*sizeof(double));
		B[i] = (double *)malloc(n*sizeof(double));
		C[i] = (double *)malloc(n*sizeof(double));
	}

	for (i = 0; i<n; i++)
		for(j=0;j<n;j++){
			A[i][j] = rand() % 5 + 1;
			B[i][j] = rand() % 5 + 1;
			C[i][j] = 0.0;
		}

	time=timer();
	i = 0; 
	j = 0;
    	/*calculates the position of each thread on a virtual grid 
          and passes them to the concerned thread */
	for (k = 0; k < t; k++){
		if(t && !(t % block_size)){
			i++;
			j = 0;	
		}
		threads_coords[k].i = i;
		threads_coords[k].j = j;
		pthread_create(&threads[k], NULL, matmul,(void*)&threads_coords[k]);  
	}

	for(k = 0; k<t; k++)
		pthread_join(threads[k], (void*)&i);
	time=timer()-time;
	printf("Elapsed time: %f \n",time/1000000.0);
	free(threads);
	free(threads_coords);
	pthread_exit(NULL);
	return 0;

}
