/*
 * roomlist.c
 * Created on: 2016-05-08
 *     Alunos: Augusto, Marcos e Jean
 */

#include "roomlist.h"

#include "sessionlist.h"

#include <stdlib.h>
#include <string.h>

int roomCmp(void *r, void *s);

RoomList* createRoomList() {
    RoomList* rl = malloc(sizeof(RoomList));
    rl->rooms = bstCreate(roomCmp);
    pthread_mutex_init(&rl->mutex, NULL);
    return rl;
}

int roomCmp(void *r, void *s) {
    return strncmp(r, s, ROOMNAME_SIZE);
}

void deleteRoomList(RoomList *rl) {
    bstDelete(rl->rooms);
    free(rl);
}

int insertRoom(RoomList *rl, char *roomname) {
    int n;
    Room *r = createRoom(roomname);
    pthread_mutex_lock(&rl->mutex);
    n = bstInsert(rl->rooms, r->roomname, r);
    pthread_mutex_unlock(&rl->mutex);
    if(n) {
        deleteRoom(r);
        return -1;
    }
    return 0;
}

Room* findRoom(RoomList *rl, char *roomname) {
    Room *r;
    pthread_mutex_lock(&rl->mutex);
    r = bstFind(rl->rooms, roomname);
    pthread_mutex_unlock(&rl->mutex);
    return r;
}

int removeRoom(RoomList *rl, char *roomname) {
    Room *s;
    pthread_mutex_lock(&rl->mutex);
    s = bstRemove(rl->rooms, roomname);
    pthread_mutex_unlock(&rl->mutex);
    if(s) return 0;
    return -1;
}
