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

/* Chat room struct */
typedef struct room {
    char roomname[ROOMNAME_SIZE + 1];   // Chat room name
    SessionList* users;                 // List of user sessions in the room
} Room;

/*
 * Creates room with name 'roomname'.
 */
Room* createRoom(char* roomname);
void deleteRoom(Room* room);

int insertUser(Room* r, Session* s);
int removeUser(Room* r, Session* s);

/*
 * Sends message to all sessions in the current room.
 */
void roomBroadcast(Room* r, Message* msg);

int roomNumUsers(Room* r);


#endif /* ROOM_H */
