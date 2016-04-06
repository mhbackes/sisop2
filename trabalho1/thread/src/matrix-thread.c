#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void multiplyRowCol(int row, int col);
int toN(int row, int col, int numCols);
int toI(int elem, int numCols);
int toJ(int elem, int numCols);
int* readMatrix(char* path, int* numRows, int* numCols);
void writeMatrix(char* path, int* M, int numRows, int numCols);
void printMatrix(FILE* file, int *M, int numRows, int numCols);

int *A, *B, *C;
int numRowsA, numColsA;
int numRowsB, numColsB;
int numRowsC, numColsC;
int size;

int getJob();
void *doWork(void *arg);
void dispatchThreads(int n);

int job;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[])
{
	if(argc < 4) {
		printf("Usage: matrix-thread nThreads A B [C]\n");
		return -1;
	}
	A = readMatrix(argv[2], &numRowsA, &numColsA);
	B = readMatrix(argv[3], &numRowsB, &numColsB);
	if(numColsA != numRowsB) {
		fprintf(stderr, "Wrong matrix dimensions\n");
		return -1;
	}
	numRowsC = numRowsA;
	numColsC = numColsB;
	size = numRowsC * numColsC;
	C = malloc(sizeof(int) * size);
	
	dispatchThreads(atoi(argv[1]));

	if(argc > 4)
		writeMatrix(argv[4], C, numRowsC, numColsC);
	else
		printMatrix(stdout, C, numRowsC, numColsC);
	return 0;
}

int toN(int row, int col, int numCols) {
	return row * numCols + col;
}

int toI(int elem, int numCols) {
	return elem / numCols;
}

int toJ(int elem, int numCols) {
	return elem % numCols;
}

void multiplyRowCol(int row, int col) {
	int i, sum = 0;
	for(i = 0; i < numColsA; i++) {
		sum += A[toN(row, i, numColsA)] * B[toN(i, col, numColsB)];
	}
	C[toN(row, col, numColsC)] = sum;
}

int* readMatrix(char* path, int* numRows, int* numCols) {
	FILE* file = fopen(path, "r");
	if(!file) {
		fprintf(stderr, "Could not open input file %s\n", path);
		exit(-1);
	}
	fscanf(file, "LINHAS = %d ", numRows);
	fscanf(file, "COLUNAS = %d", numCols);
	int size = *numRows * *numCols;
	int *M = malloc(sizeof(int) * size);
	int n;
	for(n = 0; n < size; n++) {
		fscanf(file, "%d", &M[n]);
	}
	fclose(file);
	return M;
}

void writeMatrix(char* path, int* M, int numRows, int numCols) {
	FILE* file = fopen(path, "w");
	if(!file) {
		fprintf(stderr, "Could not open output file %s\n", path);
		exit(-1);
	}
	printMatrix(file, M, numRows, numCols);
	fclose(file);
}

void printMatrix(FILE* file, int *M, int numRows, int numCols) {
	fprintf(file, "LINHAS = %d\nCOLUNAS = %d\n", numRows, numCols);
	int i, j;
	for (i = 0; i < numRows; ++i) {
		for (j = 0; j < numCols; ++j)
			fprintf(file, "%d ", M[toN(i, j, numCols)]);
		fprintf(file, "\n");
	}
}

int getJob() {
	pthread_mutex_lock(&mutex);
	int n = job++;
	pthread_mutex_unlock(&mutex);
	return n;
}

void *doWork(void *args) {
	//int id = (int) args;
	//printf("Thread %d started.\n", id);
	int job = getJob();
	while(job < size) {
		int i = toI(job, numColsC);
		int j = toJ(job, numColsC);
		//printf("Thread %d multiplying (%d, %d)\n", id, i, j);
		multiplyRowCol(i, j);
		job = getJob();
	}
	return NULL;
}

void dispatchThreads(int n) {
	pthread_t* threads = malloc(sizeof(pthread_t) * n);
	int i;
	for(i = 0; i < n; i++)
		pthread_create(&threads[i], NULL, doWork, NULL);	
	for (i = 0; i < n; i++)
		pthread_join(threads[i], NULL);
}
