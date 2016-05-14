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

/* 
 * Terminates OWN session thread and frees used memory.
 * Make sure to only execute this function using the thread created by
 * this session.
 */
void deleteSession(Session* s);

/*
 * Frees the memory allocated by session s. Does not terminate the thread.
 */
void freeSession(Session *s);

/*
 * Starts the session thread running the given function.
 */
void sessionRun(Session *s, void *thread (void*));

#endif /* SESSION_H */
