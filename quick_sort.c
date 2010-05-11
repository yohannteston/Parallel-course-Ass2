#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<pthread.h>
#include<time.h>
#include<unistd.h>
#include <sys/time.h>
 
int timer(void)
{
  struct timeval tv;
  gettimeofday(&tv, (struct timezone*)0);
  return (tv.tv_sec*1000000+tv.tv_usec);
}

void print(char* s, int* array, int size, int id){
  int i;
  //char * s = malloc(256*sizeof(char));
  printf("Thread %d: %s ",id, s);
  for(i=0;i<size; i++)
    printf("%d ",array[i]);
  printf("\n");
}

int permut(int* array, int begin, int end){
  int pivot, left, right, tmp;
  pivot = begin;
  left = begin;
  right = end;
  while(left != right){
    if(array[left] >= array[right]){
      tmp = array[left];
      array[left] = array[right];
      array[right] = tmp;
      pivot =  left+right - pivot;
    }
    if(pivot == left)
      right --;
    else
      left++;
  }
  return left;
}

void serial_quicksort(int* array, int begin, int end){
  int right,left;
  int index = permut(array, begin, end);
  if(begin < index - 1)
    serial_quicksort(array,begin, index - 1);
  if(end > index + 1)
    serial_quicksort(array,index+1, end);
}

void init(int* array, int size){
  int i;
  for(i=0;i<size;i++)
    array[i] = rand();
}

typedef struct{
  int step;
  int begin;
  int end;
  int id;
}qsort_arg;

int nb_threads;
int size;
int nb_step;

//the array to sort
int* data;

void* quicksort(void* arg){
  qsort_arg* qarg = (qsort_arg*)arg; 
  int has_spawned = 0;
  pthread_t thread;
  //printf("Thread %d, step %d: %d -> %d\n",qarg->id,qarg->step,qarg->begin,qarg->end); 
  if(qarg->step == nb_step){
    //    printf("Thread %d quicksort, step %d: %d -> %d\n",qarg->id,qarg->step,qarg->begin,qarg->end);
    //last step, each process sorts its part serially
    serial_quicksort(data, qarg->begin, qarg->end);
  }else{
    //splits the data in two set according to the pivot and spawns a new thread to sort one of the set
    qsort_arg* q_arg;
    int index = permut(data, qarg->begin, qarg->end);
    if(qarg->end > index + 1){
      q_arg = malloc(sizeof(qsort_arg));
      q_arg->step = qarg->step+1;
      q_arg->begin = index + 1;
      q_arg->end = qarg->end;
      if(pthread_create(&thread, NULL, quicksort,(void*)q_arg))
	perror("Pthread_create");
      else
	has_spawned = 1;
    }
    if(qarg->begin < index - 1){
      qarg->step++;
      qarg->end = index - 1;
      quicksort((void*)qarg);
    }
  }
  if(has_spawned)
    pthread_join(thread,NULL);
  //  printf("Thread %d left, step %d: %d -> %d\n",qarg->id,qarg->step,qarg->begin,qarg->end);
  return;
}


int main(int argc, char** argv){
	if(argc != 3){
		printf("Usage: ./quicksort size_of_array nb_threads\n");
		exit(EXIT_FAILURE); 
	}
     
	srand(time(NULL));
	int i;
	qsort_arg qarg;
	pthread_t thread;
	size = atoi(argv[1]);
	nb_threads = atoi(argv[2]);
	nb_step = (int)log2(nb_threads)+1; 

	data = malloc(size*sizeof(int));
	
	init(data,size);
	//print("main",data, size,-1);
	
	qarg.step = 1;
	qarg.begin = 0;
	qarg.end = size-1;

	int time = timer();
	
       	pthread_create(&thread,NULL,quicksort,(void*)&qarg);

	pthread_join(thread,NULL);
	
	time = timer() - time;
	printf("%f sec\n",(double)time/1000000.0);
	//print("main", data,size,-1);
	
	/* Check results, -1 implies data same as the previous element */
	for (i=0; i<size-1; i++)
	  if (data[i]>data[i+1])
	    printf("ERROR: %d (%d) > %d (%d)\n", data[i],i, data[i+1], i+1);

	free(data); 
	pthread_exit(NULL);
}
	
