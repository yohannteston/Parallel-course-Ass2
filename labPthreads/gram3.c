#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
//#define NUM_THREADS	2
int NUM_THREADS = 2;

double **V,**Q;
int n;

struct thread_data
{
	int	id;
};

struct thread_data *thread_data_array;
//struct thread_data thread_data_array[NUM_THREADS];
pthread_mutex_t *lock;

void *gs(void *threadarg);
double vecNorm(double *,int );
double scalarProd(double *,double *,int);


int main(int argc, char *argv[]) {
	int i,j,k,time,t;
	double sigma,temp_norm,norm;
	void *status;
	pthread_t *thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	n = atoi(argv[1]);
	NUM_THREADS = atoi(argv[2]);

	thread_data_array = (struct thread_data*) malloc(NUM_THREADS*sizeof(struct thread_data)) ;
	thread = (pthread_t*) malloc(NUM_THREADS * sizeof(pthread_t));

	lock=(pthread_mutex_t *)malloc(n*sizeof(pthread_mutex_t));
	for (i=0;i<n;i++) pthread_mutex_init(&lock[i], NULL);

	//Allocate and fill vectors
	V = (double **)malloc(n*sizeof(double *));
	Q = (double **)malloc(n*sizeof(double *));
	for(i=0;i<n;i++){
		V[i] = (double *)malloc(n*sizeof(double));
		Q[i] = (double *)malloc(n*sizeof(double));
	}

	for (i = 0; i<n; i++)
		for(j=0;j<n;j++)
			V[i][j] = rand() % 5 + 1;

	time=timer();

	/* 1:st Vector */
	temp_norm = vecNorm(V[0],n);
	for (k=0; k<n; k++)
		Q[0][k] = V[0][k]/temp_norm;

	for(t=0; t<NUM_THREADS-1; t++) {
		thread_data_array[t].id=t;
		pthread_create(&thread[t], &attr, gs, (void *)&thread_data_array[t]);

	}

	t=NUM_THREADS-1;
	thread_data_array[t].id=t;
	gs((void *)&thread_data_array[t]);

	for(t=0; t<NUM_THREADS-1; t++)
		pthread_join(thread[t], &status);

	time=timer()-time;
	printf("%d %f \n",NUM_THREADS, time/1000000.0);
	//printf("Check orthogonality: %e \n",scalarProd(Q[n/2], Q[n/3], n));

	return 0;

}

void *gs(void *threadarg){
	int k,j,i,id,start;
	double sigma,temp_norm;
	struct thread_data *my_data;
	my_data = (struct thread_data *) threadarg;
	id=my_data->id;

	for (j=id;j<n;j+=NUM_THREADS) pthread_mutex_lock(&lock[j]);
	if (id==0) pthread_mutex_unlock(&lock[0]);

	for (i=1;i<n;i++){
		pthread_mutex_lock(&lock[i-1]);
		pthread_mutex_unlock(&lock[i-1]);
		start=(i/NUM_THREADS)*NUM_THREADS;
		for (j=start+id;j<n;j+=NUM_THREADS){
			sigma = scalarProd(Q[i-1],V[j],n);
			for(k=0;k<n;k++)
				V[j][k] -=sigma*Q[i-1][k];
			if (i==j){
				temp_norm=vecNorm(V[i],n);
				for (k=0; k<n; k++)
					Q[i][k] = V[i][k]/temp_norm;
				pthread_mutex_unlock(&lock[i]);
			}
		}
	}
}

double vecNorm(double *vec,int n){
	int i;
	double local_norm = 0;

	for(i=0;i<n;i++){
		local_norm+= (vec[i]*vec[i]);
	}
	return sqrt(local_norm);
}

double scalarProd(double *a,double *b,int n){
	int i;
	double scalar =0.0;

	for(i=0;i<n;i++){
		scalar +=a[i]*b[i];
	}

	return scalar;
}
