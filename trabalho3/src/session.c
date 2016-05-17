/*
 * session.c
 * Created on: 2016-05-07
 *     Alunos: Augusto, Marcos e Jean
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

int sessionSendMessage(Session *s, Message *msg) {
    int n;
    pthread_mutex_lock(&s->mutex);
    n = sendMessage(s->socket, msg);
    pthread_mutex_unlock(&s->mutex);
    return n;
}

int sessionReadMessage(Session *s, Message *msg) {
    int n;
    n = readMessage(s->socket, msg);
    return n;
}
