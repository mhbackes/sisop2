/*
 * sessionlist.c
 * Created on: 2016-05-07
 *     Author: mhbackes <mhbackes@inf.ufrgs.br>
 */

#include "sessionlist.h"

#include <string.h>
#include <stdlib.h>

void sessionSendMessage(void *data, void *args);

SessionList* createSessionList() {
    SessionList *sl = malloc(sizeof(SessionList));
    sl->sessions = bstCreate(sessionCmp);
    pthread_mutex_init(&sl->mutex, NULL);
    return sl;
}

int sessionCmp(void *s, void *t) {
    return strncmp(s, t, USERNAME_SIZE);
}

void deleteSessionList(SessionList *sl) {
    bstDelete(sl->sessions);
    free(sl);
}

int insertSession(SessionList *sl, Session *s) {
    int n;
    pthread_mutex_lock(&sl->mutex);
    n = bstInsert(sl->sessions, s->username, s);
    pthread_mutex_unlock(&sl->mutex);
    return n;
}

Session* findSession(SessionList *sl, char *username) {
    Session *r;
    pthread_mutex_lock(&sl->mutex);
    r = bstFind(sl->sessions, username);
    pthread_mutex_unlock(&sl->mutex);
    return r;
}

int removeSession(SessionList *sl, Session *s) {
    Session *r;
    pthread_mutex_lock(&sl->mutex);
    r = bstRemove(sl->sessions, s->username);
    pthread_mutex_unlock(&sl->mutex);
    if(r) return 0;
    return -1;
}

void sessionListBroadcast(SessionList *sl, Message* m) {
    pthread_mutex_lock(&sl->mutex);
    bstTraverse(sl->sessions, sessionSendMessage, m);
    pthread_mutex_unlock(&sl->mutex);
}

void sessionSendMessage(void *data, void *args) {
    Session* s = (Session*) data;
    Message* msg = (Message*) args;
    sendMessage(s->socket, msg);
}
