#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/stat.h>

#include "matrix.h"

Matrix *A, *B, *C;

int cSegId;

int numProcs;

Matrix* makeSharedMatrix(int numRows, int numCols);
void freeSharedMatrix(Matrix* M);


void doWork(int row);
void dispatchProcs();

int main(int argc, char *argv[])
{
  if(argc < 2) {
    printf("Usage: matrix-thread nProcs \n");
    return -1;
  }

	numProcs = atoi(argv[1]);
	A = readMatrix(IN_1);
	B = readMatrix(IN_2);
	if(A->numCols != B->numRows) {
		fprintf(stderr, "Wrong matrix dimensions\n");
		return -1;
	}

	int size = A->numRows * B->numCols;
	cSegId = shmget(IPC_PRIVATE, size * sizeof(int), S_IRUSR | S_IWUSR);
	C = makeSharedMatrix(A->numRows, B->numCols);

	dispatchProcs();

	
	writeMatrix(OUTPUT_FILE, C);	
       
	freeMatrix(A);
	freeMatrix(B);
	freeSharedMatrix(C);
	shmctl(cSegId, IPC_RMID, NULL);
	return 0;
}

Matrix* makeSharedMatrix(int numRows, int numCols) {
	Matrix *M = malloc(sizeof(Matrix));
	M->numRows = numRows;
	M->numCols = numCols;
	M->data = shmat(cSegId, NULL, 0);
	return M;
}

void freeSharedMatrix(Matrix* M) {
	shmdt(M->data);
	free(M);
}

void dispatchProcs() {
	pid_t *pid = malloc(sizeof(pid_t) * numProcs);
	int i;
	for(i = 0; i < numProcs; i++) {
		pid[i] = fork();
		if(pid[i] < 0) {
			fprintf(stderr, "Fork failed\n");
			exit(-1);
		} else if(!pid[i]) {
			doWork(i);
			free(pid);
			exit(0);
		}
	}
	for(i = 0; i < numProcs; i++) {
		waitpid(pid[i],0,0);
	}
	free(pid);
}

void doWork(int row) {
	C->data = shmat(cSegId, NULL, 0);
	//int id = row;
	//printf("Process %d started.\n", id);
	while(row < C->numRows){
		//printf("Process %d multiplying row %d\n", id, row);
		multiplyRow(C, A, B, row);
		row += numProcs;
	}
	freeMatrix(A);
	freeMatrix(B);
	freeSharedMatrix(C);
	//printf("Process %d ended.\n", id);
}
