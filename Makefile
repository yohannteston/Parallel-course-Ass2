CC=gcc
LIBS=-lm 
FLAGS= -O3 -pthread

quick_sort: quick_sort.c
	$(CC) $(LIBS) $(FLAGS) -o quick_sort quick_sort.c

