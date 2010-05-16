#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS	5

void *HelloWorld(void *arg)
{
   int t = (int)arg;
   printf("Hello World: %d!\n",t);
   pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
pthread_t threads[NUM_THREADS];
int t;
for(t=0;t<NUM_THREADS;t++)
  pthread_create(&threads[t], NULL, HelloWorld, (void*)t);
  pthread_exit(NULL);
}
