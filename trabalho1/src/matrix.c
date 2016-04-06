#include "matrix.h"

int toN(int row, int col, int numCols) {
	return row * numCols + col;
}

int toI(int elem, int numCols) {
	return elem / numCols;
}

int toJ(int elem, int numCols) {
	return elem % numCols;
}

void multiplyRow(Matrix* C, Matrix* A, Matrix* B, int row) {
	int i;
	for(i = 0; i < B->numCols; i++) {
		multiplyRowCol(C, A, B, row, i);
	}
}

void multiplyRowCol(Matrix* C, Matrix* A, Matrix* B, int row, int col) {
	int i, sum = 0;
	for(i = 0; i < A->numCols; i++) {
		sum += A->data[toN(row, i, A->numCols)] * B->data[toN(i, col, B->numCols)];
	}
	C->data[toN(row, col, C->numCols)] = sum;
}

Matrix *makeMatrix(int numRows, int numCols) {
	Matrix *M = malloc(sizeof(Matrix));
	M->numCols = numCols;
	M->numRows = numRows;
	M->data = malloc(sizeof(int) * numCols * numRows);
	return M;
}

void freeMatrix(Matrix* M) {
	free(M->data);
	free(M);
}

Matrix* readMatrix(char* path) {
	FILE* file = fopen(path, "r");
	if(!file) {
		fprintf(stderr, "Could not open input file %s\n", path);
		exit(-1);
	}
	int numRows, numCols;
	fscanf(file, "LINHAS = %d ", &numRows);
	fscanf(file, "COLUNAS = %d", &numCols);
	Matrix *M = makeMatrix(numRows, numCols);
	int size = M->numRows * M->numCols;
	int n;
	for(n = 0; n < size; n++) {
		fscanf(file, "%d", &M->data[n]);
	}
	fclose(file);
	return M;
}

void writeMatrix(char* path, Matrix* M) {
	FILE* file = fopen(path, "w");
	if(!file) {
		fprintf(stderr, "Could not open output file %s\n", path);
		exit(-1);
	}
	printMatrix(file, M);
	fclose(file);
}

void printMatrix(FILE* file, Matrix *M) {
	fprintf(file, "LINHAS = %d\nCOLUNAS = %d\n", M->numRows, M->numCols);
	int i, j;
	for (i = 0; i < M->numRows; ++i) {
		for (j = 0; j < M->numCols; ++j)
			fprintf(file, "%d ", M->data[toN(i, j, M->numCols)]);
		fprintf(file, "\n");
	}
}
