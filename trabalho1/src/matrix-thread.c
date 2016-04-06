#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "matrix.h"

Matrix *A, *B, *C;

int numThreads;

void *doWork(void *arg);
void dispatchThreads();

int main(int argc, char *argv[])
{
	if(argc < 4) {
		printf("Usage: matrix-thread nThreads A B [C]\n");
		return -1;
	}
	numThreads = atoi(argv[1]);
	A = readMatrix(argv[2]);
	B = readMatrix(argv[3]);
	if(A->numCols != B->numRows) {
		fprintf(stderr, "Wrong matrix dimensions\n");
		return -1;
	}
	C = makeMatrix(A->numRows, B->numCols);

	dispatchThreads();

	if(argc > 4)
		writeMatrix(argv[4], C);
	else
		printMatrix(stdout, C);
	freeMatrix(A);
	freeMatrix(B);
	freeMatrix(C);
	return 0;
}


void *doWork(void *args) {
	//int id = (long) args;
	//printf("Thread %d started.\n", id);
	int row = (long) args;
	while(row < C->numRows){
		//printf("Thread %d multiplying row %d\n", id, row);
		multiplyRow(C, A, B, row);
		row += numThreads;
	}
	return NULL;
}

void dispatchThreads() {
	pthread_t* threads = malloc(sizeof(pthread_t) * numThreads);
	long i;
	for(i = 0; i < numThreads; i++)
		pthread_create(&threads[i], NULL, doWork, (void*) i);	
	for (i = 0; i < numThreads; i++)
		pthread_join(threads[i], NULL);
}
