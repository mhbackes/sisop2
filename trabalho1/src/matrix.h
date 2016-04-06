#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int numRows, numCols, *data;
} Matrix;

/**
 * Multiplica a linha "row" da matriz A, por todas as colulas da matriz B
 * e armazena os resultados na matriz C.
 */
void multiplyRow(Matrix* C, Matrix* A, Matrix* B, int row);

/**
 * Multiplica a linha "row" de A pela coluna "col" de B e armazena o resultado
 * em C[row, col].
 */
void multiplyRowCol(Matrix* C, Matrix* A, Matrix* B, int row, int col);

/**
 * Converte coordenadas bidimensionais de uma matriz em coordenadas unidimen-
 * sionais de um vetor.
 */
int toN(int row, int col, int numCols);

Matrix *makeMatrix(int numRows, int numCols);

Matrix *readMatrix(char *path);

void writeMatrix(char *path, Matrix *M);

void printMatrix(FILE *file, Matrix *M);

#endif
