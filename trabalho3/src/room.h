/*
 * room.h
 * Created on: 2016-05-06
 *	   Author: mhbackes <mhbackes@inf.ufrgs.br>
 */

#ifndef ROOM_H
#define ROOM_H

#include <pthread.h>
#include "session.h"
#include "sessionlist.h"
#include "bstree.h"

#define ROOMNAME_SIZE 31

/* Chat room struct */
typedef struct room {
    char roomname[ROOMNAME_SIZE + 1];   // Chat room name
    SessionList* users;             // List of user sessions in the room
    pthread_mutex_t mutex;          // Chat room mutex
} Room;

Room* createRoom(char* roomname);
void deleteRoom(Room* room);
int insertUser(Room* r, Session* s);
int removeUser(Room* r, Session* s);
void roomBroadcast(Room* r, Message* msg);
int roomNumUsers(Room* r);


#endif /* ROOM_H */
