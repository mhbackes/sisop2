/*
 * command.c
 * Created on: 2016-05-07
 *     Author: mhbackes <mhbackes@inf.ufrgs.br>
 */

#include "command.h"

#include <stdio.h>

SessionList *onlineUsers;
Room* room; // TODO multiple rooms

void *sessionThread(void *args);
int commandLogout(Session *s);
int commandChat(Session *s, Message *msg);

void serverInit() {
    onlineUsers = createSessionList();
    room = createRoom("A room");
}

int startSession(int socket) {
    Message msg;
    readMessage(socket, &msg);
    Session *s = createSession(socket, msg.username);
    if(insertSession(onlineUsers, s)) {
        printf("Username already taken.\n");
        deleteSession(s);
        return -1;
    } else {
        insertUser(room, s);
        sessionRun(s, sessionThread);
        return 0;
    }
}

void *sessionThread(void *args) {
    Session *s = (Session*) args;
    Message msg;
    do {
        readMessage(s->socket, &msg);
        execute(s, &msg);
    } while(msg.type != MSG_LOGOUT);
    return NULL;
}


int execute(Session *s, Message *msg) {
    switch(msg->type) {
        case MSG_LOGOUT:
            return commandLogout(s);
        case MSG_CHAT:
            return commandChat(s, msg);
        default:
            return -1;
    }
}

int commandLogout(Session *s) {
    if(s->room) removeUser(s->room, s);   
    removeSession(onlineUsers, s);
    pthread_detach(s->thread);
    deleteSession(s);
    return 0;
}

int commandChat(Session *s, Message *msg) {
    roomBroadcast(s->room, msg);
    return 0;
}
