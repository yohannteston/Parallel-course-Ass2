/**********************************************************************
 * This program calculates pi using C
 *
 **********************************************************************/
#include <stdio.h>
#include <pthread.h>
#define NB_THREADS 20

const int intervals = 100000000L ;
int slice;
double dx; 
double glob_sum;
pthread_mutex_t lock;



void* pi(void* arg){
	int i;
	int end;
	int tid = (int)arg;
	double x, sum;	

	sum = 0;

	if(tid == NB_THREADS-1)
		//we make sure the last step stops exactly at intervals 
		end = intervals;
	else
		end = tid*slice+slice;
	/* computing pi */
	for (i = tid*slice+1; i <= end; i++) { 
		x = dx*(i - 0.5);
		sum += dx*4.0/(1.0 + x*x);
	}
	//a mutex is needed to protect the global variable
	pthread_mutex_lock(&lock);
	glob_sum += sum;
	pthread_mutex_unlock(&lock);	
	pthread_exit(NULL);
}


int main(int argc, char *argv[]) {

	int status, i;
	pthread_t threads[NB_THREADS];

	pthread_mutex_init(&lock, NULL);

	slice = intervals/NB_THREADS;
	dx = 1.0/intervals;
	glob_sum = 0.0;

	for (i = 0; i < NB_THREADS; i++) { 
		pthread_create(&threads[i],NULL,pi, (void*)i);
	}
	for (i = 0; i < NB_THREADS; i++) { 
		pthread_join(threads[i],(void*)&status);
	}

	pthread_mutex_destroy(&lock);
	printf("PI is approx. %.16f\n",  glob_sum);
	pthread_exit(NULL);
	return 0;
}
