#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<pthread.h>
#include<time.h>
#include<unistd.h>
#include <sys/time.h>

void swap(int* A, int i, int j){
  int k = A[i];
  A[i] = A[j];
  A[j] = k;
}

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

//any pivot selection algorithm can be implemented here. Must return the pivot's index. For this assignment, we chose to use the median of three strategy, that is the pivot is the median of begin, end and the middle value of the interval.
int choose_pivot(int* array, int begin, int end){
  int tmp, middle = (begin+end)/2;
  //return begin;
  // printf("%d, %d, %d\n",array[begin], array[middle], array[end]);
  if(array[begin] > array[middle]){
    //since both begin and middle are used later, we need to swap them properly
    tmp = begin;
    begin = middle;
    middle = tmp;
  }
  if(array[begin] > array[end])
    end = begin;
  if(array[middle] > array[end])
    middle = end;
  //  printf("chosen: %d\n",array[middle]);
  return middle;
}

int permut(int* array, int begin, int end){
  int pivot;
    //for this algorithm to work, the pivot must be the first element. So, we swap it with the first element
  swap(array, choose_pivot(array, begin, end), begin);
  pivot = begin;

  while(begin != end){
    if(array[begin] >= array[end]){
      swap(array, begin,end);
      pivot =  begin+end - pivot;
    }
    if(pivot == begin)
      end --;
    else
      begin++;
  }
  return begin;
}

void serial_quicksort(int* array, int begin, int end){
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

int size;
int nb_steps;

//the array to sort
int* data;

void* quicksort(void* arg){
  qsort_arg* qarg = (qsort_arg*)arg; 
  int has_spawned = 0;
  pthread_t thread;
  //printf("Thread %d, step %d: %d -> %d\n",qarg->id,qarg->step,qarg->begin,qarg->end); 
  if(qarg->step >= nb_steps){
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
      else{
	has_spawned = 1;
      }
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
		printf("Usage: ./quicksort size_of_array max_steps\n");
		exit(EXIT_FAILURE); 
	}
     
	srand(time(NULL));
	int i;
	qsort_arg qarg;
	pthread_t thread;
	size = atoi(argv[1]);
	
	if(size <= 0){
	  printf("At least, give us some data to work with...\n");
	  return EXIT_FAILURE;
	}

	nb_steps = atoi(argv[2]);
	
	if(nb_steps < 0){
	  printf("A negative number of steps is impossible...\n");
	  return EXIT_FAILURE;
	} 

	data = malloc(size*sizeof(int));
	
	init(data,size);
	//print("main",data, size,-1);
	
	qarg.step = 0;
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
	
