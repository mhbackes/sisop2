/*
 * roomlist.h
 * Created on: 2016-05-06
 *	   Author: mhbackes <mhbackes@inf.ufrgs.br>
 */

#ifndef ROOMLIST_H
#define ROOMLIST_H

#include <pthread.h>

#include "bstree.h"
#include "room.h"

typedef struct roomlist {
    BSTree* rooms;
    pthread_mutex_t mutex;
} RoomList;

RoomList* createRoomList();
void deleteRoomList(RoomList *rl);

int insertRoom(RoomList *rl, char *roomname);
Room* findRoom(RoomList *rl, char *roomname);
int removeRoom(RoomList *rl, char *roomname);

//void printRoomList(RoomList *rl);

#endif /* ROOMLIST_H */
