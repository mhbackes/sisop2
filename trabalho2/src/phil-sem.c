#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define PHIL_THINK_TIME 10
#define PHIL_EAT_TIME 10

typedef struct philosopher_t { 
	pthread_t thread;
	int id;					// phil's id number
	int state;				// phil's state (THINKING, HUNGRY, EATING)
	sem_t *left, *right;	// phil's forks
} Philosopher;

sem_t *forks;
Philosopher *phils;
int numPhils;

enum State {
	IDLE, THINKING, HUNGRY, EATING
};

const char stateToChar[] = {
	'-', 'T', 'H', 'E'
};

void initForks();
void initPhils();
void serveDinner();

void *philDine(void *args);
void philThink(Philosopher *p);
void philHungry(Philosopher *p);
void philEat(Philosopher *p);

void philChangeState(Philosopher *p, int state);
void philPrintStates();
void philGrabForks(Philosopher *p);
void philReleaseForks(Philosopher *p);

int main(int argc, char *argv[]) {
	if(argc < 2) {
		fprintf(stderr, "Usage: %s [numPhils]\n", argv[0]);
		exit(1);
	}
	numPhils = atoi(argv[1]);
	initForks();
	initPhils();
	serveDinner();
	return 0;
}

void initForks() {
	forks = malloc(sizeof(sem_t) * numPhils);
	int i;
	for(i = 0; i < numPhils; i++)
		sem_init(&forks[i], 0, 1);
}

void initPhils() {
	phils = malloc(sizeof(Philosopher) * numPhils);
	int i;
	Philosopher *p;
	for(i = 0; i < numPhils; i++) {
		p = &phils[i];
		p->state = IDLE;
		p->id = i;
		p->left = &forks[i];
		p->right = &forks[(i+1) % numPhils];
	}
}

void serveDinner() {
	int i;
	for(i = 0; i < numPhils; i++) {
		pthread_create(&phils[i].thread, NULL, philDine, &phils[i]);
	}
	for(i = 0; i < numPhils; i++) {
		pthread_join(phils[i].thread, NULL);
	}
}

void *philDine(void *args) {
	Philosopher *myself = (Philosopher *) args;
	while(1) {
		philThink(myself);
		philHungry(myself);
		philEat(myself);
	}
}

void philThink(Philosopher *p) {
	philChangeState(p, THINKING);
	int thinkingTime = rand() % PHIL_THINK_TIME;
	//printf("Philosopher %d thinks: \"I think for %ds, therefore I am.\"\n", p->id, thinkingTime);
	sleep(thinkingTime);
}

void philHungry(Philosopher *p) {
	philChangeState(p, HUNGRY);
	//printf("Philosopher %d thinks: \"I think, therefore I am hungry.\"\n", p->id);
	philGrabForks(p);
}

void philEat(Philosopher *p) {
	philChangeState(p, EATING);
	int eatingTime = rand() % PHIL_EAT_TIME;
	//printf("Philosopher %d thinks: \"I think, therefore I am eating for %ds.\"\n", p->id, eatingTime);
	sleep(eatingTime);
	philReleaseForks(p);
}

void philChangeState(Philosopher *p, int state) {
	static pthread_mutex_t smutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_lock(&smutex);
	p->state = state;
	philPrintStates();
	pthread_mutex_unlock(&smutex);
}

void philPrintStates() {
	int i;
	for(i = 0; i < numPhils; i++) {
		printf("%c ", stateToChar[phils[i].state]);	
	}
	printf("\n");
}

void philGrabForks(Philosopher *p) {
	static pthread_mutex_t fmutex = PTHREAD_MUTEX_INITIALIZER;	
	pthread_mutex_lock(&fmutex);
	sem_wait(p->left);
	sem_wait(p->right);
	pthread_mutex_unlock(&fmutex);
}

void philReleaseForks(Philosopher *p) {
	sem_post(p->left);
	sem_post(p->right);
}
