/*
 * room.c
 * Created on: 2016-05-07
 *     Author: mhbackes <mhbackes@inf.ufrgs.br>
 */

#include "room.h"

#include <stdlib.h>
#include <string.h>

int sessionCmp(void *s, void *t);

Room* createRoom(char* roomname) {
    Room* r = malloc(sizeof(Room));
    strncpy(r->roomname, roomname, ROOMNAME_SIZE);
    r->users = createSessionList();
    pthread_mutex_init(&r->mutex, NULL);
    return r;
}

void deleteRoom(Room* r) {
    deleteSessionList(r->users);
    free(r);
}

int insertUser(Room* r, Session* s) {
    if(insertSession(r->users, s))
        return -1;
    s->room = r;
    return 0;
}

int removeUser(Room* r, Session* s) {
    if(removeSession(r->users, s))
        return -1;
    s->room = NULL;
    return 0;
}

void roomBroadcast(Room* r, Message* msg) {
    sessionListBroadcast(r->users, msg);
}
