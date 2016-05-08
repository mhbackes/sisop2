/*
 * sessionlist.h
 * Created on: 2016-05-06
 *	   Author: mhbackes <mhbackes@inf.ufrgs.br>
 */

#ifndef SESSIONLIST_H
#define SESSIONLIST_H

#include <pthread.h>

#include "bstree.h"
#include "session.h"
#include "message.h"

typedef struct sessionlist {
    BSTree* sessions;
    pthread_mutex_t mutex;
} SessionList;

SessionList* createSessionList();
void deleteSessionList(SessionList *sl);

int insertSession(SessionList *sl, Session *s);
Session* findSession(SessionList *sl, char *username);
int removeSession(SessionList *sl, Session *s);
void sessionListBroadcast(SessionList *sl, Message* m);

int sessionNumUsers(SessionList *sl);

#endif /* SESSIONLIST_H */
