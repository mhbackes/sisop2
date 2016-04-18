#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define PHIL_THINK_TIME 10
#define PHIL_EAT_TIME 10

// philosopher's states
enum PhilState { THINKING, HUNGRY, EATING };

// philosopher struct
typedef struct philosopher_t { 
  pthread_t thread;
  int id;					// philosopher's id number
  int prio;					// philosopher's eating priority
  int state;					// philosopher's state (THINKING, HUNGRY, EATING)
  //pthread_cond_t *forkLeft, *forkRight;		// philosopher's forks
  struct philosopher_t *philLeft, *philRight;	// philosopher's neighbors
} Philosopher;

pthread_cond_t *forks;
pthread_cond_t forkLock = PTHREAD_COND_INITIALIZER;
Philosopher *phils;
int numPhils;

void setTheTable();
void invitePhilosophers();
void serveDinner();
void cleanDishes();
void killPhilosophers();

void *	philDine(void *args);
void	philThink(Philosopher *p);
void	philHungry(Philosopher *p);
void	philEat(Philosopher *p);

void	philChangeState(Philosopher *p, int state);
void	philPrintStates();
void	philGrabForks(Philosopher *p);
int	philCanEat(Philosopher *p);
void	philReleaseForks(Philosopher *p);

int randomInt(int rangeMin, int rangeSize);
int count;

int main(int argc, char *argv[]) {

	if(argc < 2) {
		fprintf(stderr, "Usage: %s [numPhils]\n", argv[0]);
		exit(1);
	}

	numPhils = atoi(argv[1]);
	count = 0;

	setTheTable();

	invitePhilosophers();
	serveDinner();

	cleanDishes();
	killPhilosophers();

	return 0;
}

void setTheTable() {
	forks = malloc(sizeof(pthread_cond_t) * numPhils);
	for(int i = 0; i < numPhils; i++)
		pthread_cond_init(&forks[i], NULL);
}

void invitePhilosophers() {
	phils = malloc(sizeof(Philosopher) * numPhils);
	for(int i = 0; i < numPhils; i++) {
		Philosopher *p = &phils[i];
		p->state = THINKING;
		p->id = i;
		p->prio = 0;
		//p->forkLeft = &forks[i];
		//p->forkRight = &forks[(i+1) % numPhils];
		p->philLeft = &phils[(i+numPhils-1) % numPhils];
		p->philRight = &phils[(i+1) % numPhils];
	}
	// random seed
	srand(time(NULL));
}

void cleanDishes() {
	free(forks);
}

void killPhilosophers() {
	free(phils);
}

void serveDinner() {
	philPrintStates();
	for(int i = 0; i < numPhils; i++) {
		pthread_create(&phils[i].thread, NULL, philDine, &phils[i]);
	}
	for(int i = 0; i < numPhils; i++) {
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
	return NULL;
}

void philThink(Philosopher *p) {
	int thinkingTime = randomInt(1, PHIL_THINK_TIME);
	//printf("Philosopher %d thinks: \"I think for %ds, therefore I am.\"\n", p->id, thinkingTime);
	sleep(thinkingTime);
}

void philHungry(Philosopher *p) {
	//printf("Philosopher %d thinks: \"I think, therefore I am hungry.\"\n", p->id);
	philGrabForks(p);
}

void philEat(Philosopher *p) {
	int eatingTime = randomInt(1, PHIL_EAT_TIME);
	//printf("Philosopher %d thinks: \"I think, therefore I am eating for %ds.\"\n", p->id, eatingTime);
	sleep(eatingTime);
	philReleaseForks(p);
}

void philChangeState(Philosopher *p, int state) {
	static pthread_mutex_t smutex = PTHREAD_MUTEX_INITIALIZER;
	// mutex only necessary to make sure printing is correct
	pthread_mutex_lock(&smutex);
	p->state = state;

	// whenever the philosopher changes his/her state, he/she resets his/her priority
	p->prio = 0;

	philPrintStates();

	pthread_mutex_unlock(&smutex);
}

void philPrintStates() {
	static const char stateToChar[] = { 'T', 'H', 'E' };
	for(int i = 0; i < numPhils; i++) {
		printf("%c ", stateToChar[phils[i].state]);	
	}
	printf("\n");
}

void philGrabForks(Philosopher *p) {
	static pthread_mutex_t fmutex = PTHREAD_MUTEX_INITIALIZER;

	pthread_mutex_lock(&fmutex);

	philChangeState(p, HUNGRY);
	// if the philosopher can't eat, he/she blocks himself/herself
	while(!philCanEat(p)) 
	  pthread_cond_wait(&forkLock, &fmutex);

	// grabs forks
	//pthread_cond_wait(p->forkLeft, &fmutex);
	//pthread_cond_wait(p->forkRight, &fmutex);
	philChangeState(p, EATING);

	// signals next philosopher after grabbing forks
	pthread_cond_signal(&forkLock);
	pthread_mutex_unlock(&fmutex);
}

int philCanEat(Philosopher *p) {
	int selfPrio = p->prio;
	int leftState = p->philLeft->state;
	int leftPrio = p->philLeft->prio;
	int rightState = p->philRight->state;
	int rightPrio = p->philRight->prio;

	// philosopher is able to eat if none of his neighbors is eating
	int canEat = leftState != EATING && rightState != EATING;
	
	// philosopher should not eat if his/her neighbors are hungry and have 
	// higher priority
	int shouldEat = (leftState == THINKING || (leftState == HUNGRY && selfPrio >= leftPrio)) &&
					(rightState == THINKING || (rightState == HUNGRY && selfPrio >= rightPrio));

	// whenever the philosopher is unable to eat, his/her priority increases
	if(!canEat)	
	  p->prio++;

	// if the philosopher shouldn't eat, he/she signals the next philosopher, 
	// so his/her hungry neighbor has a chance to eat
	if(!shouldEat) 
	  pthread_cond_signal(&forkLock);

	// philosopher will only grab both forks if he/she can eat and he/she should eat
	return canEat && shouldEat;
}

void philReleaseForks(Philosopher *p) {
	//pthread_cond_signal(p->forkLeft);
	//pthread_cond_signal(p->forkRight);
	philChangeState(p, THINKING);
}

int randomInt(int rangeMin, int rangeSize) {
	return rand() % rangeSize + rangeMin;
}
