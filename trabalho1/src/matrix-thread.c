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

	if(argc < 2) {
		printf("Usage: matrix-thread nThreads\n");
		return -1;
	}
	numThreads = atoi(argv[1]);
	A = readMatrix(IN_1);
	B = readMatrix(IN_2);
	if(A->numCols != B->numRows) {
		fprintf(stderr, "Wrong matrix dimensions\n");
		return -1;
	}
	C = makeMatrix(A->numRows, B->numCols);

	dispatchThreads();

	writeMatrix(OUTPUT_FILE, C);

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
