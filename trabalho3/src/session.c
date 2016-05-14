/*
 * session.c
 * Created on: 2016-05-07
 *     Author: mhbackes <mhbackes@inf.ufrgs.br>
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "session.h"

void *sessionThread(void *args);

Session* createSession(int socket, char* username) {
    Session *s = malloc(sizeof(Session));
    if(!s) {
        fprintf(stderr, "Memory error\n");
        exit(-1);
    }
    s->socket = socket;
    strncpy(s->username, username, USERNAME_SIZE);
    s->room = NULL;
    return s;
}

void deleteSession(Session* s) {
	freeSession(s);
    pthread_exit(NULL);
}

void freeSession(Session *s) {
	close(s->socket);
	free(s);
}

void sessionRun(Session *s, void *thread (void*)) {
    pthread_create(&s->thread, NULL, thread, s);
}
