/*
 * command.c
 * Created on: 2016-05-07
 *     Author: mhbackes <mhbackes@inf.ufrgs.br>
 */

#include "command.h"

#include <stdio.h>
#include <string.h>

SessionList *_onlineUsers;
RoomList *_rooms;

void *sessionThread(void *args);

int commandLogout(Session *s);
int commandChat(Session *s, Message *msg);
int commandName(Session *s, Message *msg);
int commandCreateRoom(Session *s, Message *msg);
int commandDeleteRoom(Session *s, Message *msg);
int commandJoinRoom(Session *s, Message *msg);
int commandLeaveRoom(Session *s, Message *msg);

void serverInit() {
    _onlineUsers = createSessionList();
    _rooms = createRoomList();
}

int startSession(int socket) {
    Message msg;
    readMessage(socket, &msg);
    Session *s = createSession(socket, msg.username);
    if(insertSession(_onlineUsers, s)) {
        serverMessage(&msg, MSG_ERROR, "Username already taken");
        sendMessage(socket, &msg);
        deleteSession(s);
        return -1;
    } else {
        serverMessage(&msg, MSG_SUCCESS, "");
        sendMessage(socket, &msg);
        sessionRun(s, sessionThread);
        return 0;
    }
}

void *sessionThread(void *args) {
    Session *s = (Session*) args;
    Message msg;
    printf("Client \"%s\" connected.\n", s->username);
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
        case MSG_NAME:
            return commandName(s, msg);
        case MSG_CREATE_ROOM:
            return commandCreateRoom(s, msg);
        case MSG_JOIN_ROOM:
            return commandJoinRoom(s, msg);
        default:
            fprintf(stdout, "Unknown command.\n");
            return -1;
    }
}

int commandLogout(Session *s) {
    if(s->room) removeUser(s->room, s);   
    removeSession(_onlineUsers, s);
    fprintf(stdout, "Client \"%s\" disconnected.\n", s->username);
    pthread_detach(s->thread);
    deleteSession(s);
    return 0;
}

int commandChat(Session *s, Message *msg) {
    Message rmsg;
    strncpy(msg->username, s->username, USERNAME_SIZE);
    if(!s->room) {
        serverMessage(&rmsg, MSG_ERROR, "You must enter a room to chat.");
        sendMessage(s->socket, &rmsg);
        return -1;
    }
    roomBroadcast(s->room, msg);
    fprintf(stdout, "Room: \"%s\", Client: \"%s\", Message: \"%s\".\n", 
            s->room->roomname, s->username, msg->text);
    return 0;
}

int commandName(Session *s, Message *msg) {
    Message rmsg;
    if(findSession(_onlineUsers, msg->text)) {
        serverMessage(&rmsg, MSG_ERROR, "Username already taken\n");
        sendMessage(s->socket, &rmsg);
        return -1;
    }
    removeSession(_onlineUsers, s);
    strncpy(s->username, msg->text, USERNAME_SIZE);
    insertSession(_onlineUsers, s);
    serverMessage(&rmsg, MSG_SUCCESS, "");
    sendMessage(s->socket, &rmsg);
    fprintf(stdout, "Client \"%s\" changed name to \"%s\".\n",
            msg->username, s->username);
    return 0;
}

int commandCreateRoom(Session *s, Message *msg) {
    Message rmsg;
    if(insertRoom(_rooms, msg->text)) {
        serverMessage(&rmsg, MSG_ERROR, "Room already exists");
        sendMessage(s->socket, &rmsg);
        return -1;
    }
    serverMessage(&rmsg, MSG_SUCCESS, "");
    sendMessage(s->socket, &rmsg);
    fprintf(stdout, "Client \"%s\" created room \"%s\".\n",
            s->username, msg->text);
    return 0;
}

int commandDeleteRoom(Session *s, Message *msg); //TODO

int commandJoinRoom(Session *s, Message *msg) {
    Message rmsg;
    Room *r = findRoom(_rooms, msg->text);
    if(!r) {
        serverMessage(&rmsg, MSG_ERROR, "Room does not exist");
        sendMessage(s->socket, &rmsg);
        return -1;
    }
    insertUser(r, s);
    serverMessage(&rmsg, MSG_SUCCESS, "");
    sendMessage(s->socket, &rmsg);
    fprintf(stdout, "Client \"%s\" joined room \"%s\".\n",
            s->username, msg->text);
    return 0;
    
}

int commandLeaveRoom(Session *s, Message *msg); //TODO
