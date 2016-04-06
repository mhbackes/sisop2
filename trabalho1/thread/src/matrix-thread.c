#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

int main(int argc, char *argv[])
{
	if(argc < 3) {
		printf("Usage: matrix-thread A B [C]\n");
		return -1;
	}
	A = readMatrix(argv[1], &numRowsA, &numColsA);
	B = readMatrix(argv[2], &numRowsB, &numColsB);
	if(numColsA != numRowsB) {
		fprintf(stderr, "Wrong matrix dimensions\n");
		return -1;
	}
	numRowsC = numRowsA;
	numColsC = numColsB;
	C = malloc(sizeof(int) * numRowsC * numColsC);
	int i, j;
	for (i = 0; i < numRowsC; ++i) {
		for (j = 0; j < numColsC; ++j)
			multiplyRowCol(i, j);
	}
	if(argc > 3)
		writeMatrix(argv[3], C, numRowsC, numColsC);
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
	fscanf(file, "%d %d", numRows, numCols);
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
	fprintf(file, "%d %d\n", numRows, numCols);
	int i, j;
	for (i = 0; i < numRows; ++i) {
		for (j = 0; j < numCols; ++j)
			fprintf(file, "%d ", M[toN(i, j, numCols)]);
		fprintf(file, "\n");
	}
}
