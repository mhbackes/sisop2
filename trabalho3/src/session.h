/*
 * session.h
 * Created on: 2016-05-06
 *	   Author: mhbackes <mhbackes@inf.ufrgs.br>
 */

#ifndef SESSION_H
#define SESSION_H

#include <pthread.h>

#define USERNAME_SIZE 31

struct room;

/* User session struct */
typedef struct session {
    int socket;                         // user current socket
    pthread_t thread;                   // session thread
    char username[USERNAME_SIZE + 1];   // user name
    struct room* room;                  // user's current room
} Session;

Session* createSession(int socket, char* username);
void deleteSession(Session* s);
void sessionRun(Session *s, void *thread (void*));

#endif /* SESSION_H */
