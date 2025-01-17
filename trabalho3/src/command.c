/*
 * command.c
 * Created on: 2016-05-07
 *     Alunos: Augusto, Marcos e Jean
 */

#include "command.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

SessionList *_onlineUsers;
RoomList *_rooms;

// Session thread basic routine
void *sessionThread(void *args);

// Comands
int commandLogout(Session *s);
int commandChat(Session *s, Message *msg);
int commandName(Session *s, Message *msg);
int commandCreateRoom(Session *s, Message *msg);
int commandDeleteRoom(Session *s, Message *msg);
int commandJoinRoom(Session *s, Message *msg);
int commandLeaveRoom(Session *s);
int commandUnknown(Session *s, Message *msg);
int commandHelp(Session *s);
int commandClear(Session *s);
int commandLS(Session *s);
int commandPrivate(Session *s, Message *msg);

void printRoomList(BSTNode *x, Session *s);
int checkName(char* username);

/*
 * Ends session if error on socket.
 */
void serverReadMessage(Session *s, Message *msg);
/*
 * Ends session if error on socket.
 */
void serverSendMessage(Session *s, Message *msg);

void serverInit() {
    _onlineUsers = createSessionList();
    _rooms = createRoomList();
}

int startSession(int socket) {
    Message msg;
    readMessage(socket, &msg);
    Session *s = createSession(socket, msg.username);
    if(checkName(msg.username)) {
        serverMessage(&msg, MSG_ERROR, "Bad username.");
        sessionSendMessage(s, &msg);
        freeSession(s);
        return -1;
    }
    if(insertSession(_onlineUsers, s)) {
        serverMessage(&msg, MSG_ERROR, "Username already taken.");
        sessionSendMessage(s, &msg);
        freeSession(s);
        return -1;
    } else {
        serverMessage(&msg, MSG_SUCCESS, "Welcome.");
        sessionSendMessage(s, &msg);
        sessionRun(s, sessionThread);
        return 0;
    }
}

void *sessionThread(void *args) {
    Session *s = (Session*) args;
    Message msg;
    printf("Client \"%s\" connected.\n", s->username);
    do {
        serverReadMessage(s, &msg);
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
        case MSG_DELETE_ROOM:
            return commandDeleteRoom(s, msg);
        case MSG_JOIN_ROOM:
            return commandJoinRoom(s, msg);
        case MSG_LEAVE_ROOM:
            return commandLeaveRoom(s);
        case MSG_HELP:
            return commandHelp(s);
        case MSG_CLEAR:
            return commandClear(s);
        case MSG_LS:
            return commandLS(s);
        case MSG_PVT:
            return commandPrivate(s, msg);
        default:
            return commandUnknown(s, msg);
    }
}

int commandLogout(Session *s) {
    if(s->room) removeUser(s->room, s);   
    removeSession(_onlineUsers, s);
    fprintf(stdout, "Client \"%s\" disconnected.\n", s->username);
    deleteSession(s);
    return 0;
}

int commandChat(Session *s, Message *msg) {
    Message rmsg;
    strncpy(msg->username, s->username, USERNAME_SIZE);
    if(!s->room) {
        serverMessage(&rmsg, MSG_ERROR, "You must enter a room to chat.");
        serverSendMessage(s, &rmsg);
        return -1;
    }
    roomBroadcast(s->room, msg);
    fprintf(stdout, "Client \"%s\" sent message \"%s\" to room \"%s\".\n", 
            s->username, msg->text, s->room->roomname);
    return 0;
}

int commandName(Session *s, Message *msg) {
    Message rmsg;
    if(s->room) {
        serverMessage(&rmsg, MSG_ERROR,
                "You must leave the room to change your name.");
        serverSendMessage(s, &rmsg);
        return -1;
    }
    if(checkName(msg->text)) {
        serverMessage(&rmsg, MSG_ERROR, "Bad username.");
        serverSendMessage(s, &rmsg);
        return -1;
    }
    if(findSession(_onlineUsers, msg->text)) {
        serverMessage(&rmsg, MSG_ERROR, "Username already taken.");
        serverSendMessage(s, &rmsg);
        return -1;
    }
    removeSession(_onlineUsers, s);
    strncpy(s->username, msg->text, USERNAME_SIZE);
    insertSession(_onlineUsers, s);
    serverMessage(&rmsg, MSG_SUCCESS, "Name changed");
    serverSendMessage(s, &rmsg);
    fprintf(stdout, "Client \"%s\" changed name to \"%s\".\n",
            msg->username, s->username);
    return 0;
}

int checkName(char* name) {
    int i = 0;
    while(name[i]) {
        if(!isalnum(name[i++]))
            return -1;
    }
    if(!*name)
        return -1;
    if(!strncmp(name, "SERVER", USERNAME_SIZE))
        return -1;
    if(strstr(name, " "))
        return -1;
    return 0;
}

int commandCreateRoom(Session *s, Message *msg) {
    Message rmsg;
    if(checkName(msg->text)) {
        serverMessage(&rmsg, MSG_ERROR, "Bad roomname.");
        serverSendMessage(s, &rmsg);
        return -1;
    }
    if(insertRoom(_rooms, msg->text)) {
        serverMessage(&rmsg, MSG_ERROR, "Room already exists.");
        serverSendMessage(s, &rmsg);
        return -1;
    }
    serverMessage(&rmsg, MSG_SUCCESS, "Room created.");
    serverSendMessage(s, &rmsg);
    fprintf(stdout, "Client \"%s\" created room \"%s\".\n",
            s->username, msg->text);
    return 0;
}

int commandDeleteRoom(Session *s, Message *msg) {
    Message rmsg;
    Room* r = findRoom(_rooms, msg->text);
    if(!r) {
        serverMessage(&rmsg, MSG_ERROR, "Room does not exist.");
        serverSendMessage(s, &rmsg);
        return -1;
    }
    if(roomNumUsers(r)) {
        serverMessage(&rmsg, MSG_ERROR, "Room is not empty.");
        serverSendMessage(s, &rmsg);
        return -1;
    }
    if(!removeRoom(_rooms, msg->text)) 
        deleteRoom(r);
    serverMessage(&rmsg, MSG_SUCCESS, "Room deleted.");
    serverSendMessage(s, &rmsg);
    fprintf(stdout, "Client \"%s\" deleted room \"%s\".\n",
            s->username, msg->text);
    return 0;
}

int commandJoinRoom(Session *s, Message *msg) {
    Message rmsg;
    Room *r = findRoom(_rooms, msg->text);
    if(!r) {
        serverMessage(&rmsg, MSG_ERROR, "Room does not exist.");
        serverSendMessage(s, &rmsg);
        return -1;
    }
    if(s->room)
        removeUser(s->room, s);
    insertUser(r, s);
    fprintf(stdout, "Client \"%s\" joined room \"%s\".\n",
            s->username, msg->text);
    return 0;
}

int commandLeaveRoom(Session *s) {
    Message rmsg;
    Room* r = s->room;
    if(!r) {
        serverMessage(&rmsg, MSG_ERROR, "You are not in a room.");
        serverSendMessage(s, &rmsg);
        return -1;
    }
    removeUser(r, s);
    serverMessage(&rmsg, MSG_SUCCESS, "You left the room.");
    serverSendMessage(s, &rmsg);
    fprintf(stdout, "Client \"%s\" left room \"%s\".\n",
            s->username, r->roomname);
    return 0;
}

int commandUnknown(Session *s, Message* msg) {
    Message rmsg;
    serverMessage(&rmsg, MSG_ERROR, "Unknown command.");
    serverSendMessage(s, &rmsg);
    fprintf(stdout, "Client \"%s\" sent unknown command: id %d.\n",
            s->username, msg->type);
    return 0;

}

int commandHelp(Session *s) {
    Message rmsg;
    serverMessage(&rmsg, MSG_HELP, "You asked for HELP. :)\nList of commands.\n\t\\create <room> : Creates a new room named <room>. \n\t\\join <room>   : Joins <room>. \n\t\\name <name>   : Changes the user name to <name>. \n\t\\leave         : User leaves the current room. \n\t\\delete <room> : User deletes the room named <room>. \n\t\\quit          : User quits the chat app. \n\t\\ls            : Lists all available rooms.\n\t\\clear         : Screen is clear.\n\t@<username>    : Sends a private message to <username>.");
    serverSendMessage(s, &rmsg);
    return 0;
}

int commandClear(Session *s) {
    Message rmsg;
    serverMessage(&rmsg, MSG_CLEAR, "\n");
    serverSendMessage(s, &rmsg);
    return 0;
}

int commandLS(Session *s) {
    Message rmsg;
    serverMessage(&rmsg, MSG_LS, "LIST OF ROOMS - ");
    serverSendMessage(s, &rmsg);
    BSTNode *x = _rooms->rooms->root;
    pthread_mutex_lock(&_rooms->mutex);
    pthread_mutex_lock(&s->mutex);
    printRoomList(x, s);
    pthread_mutex_unlock(&s->mutex);
    pthread_mutex_unlock(&_rooms->mutex);
    return 0;
}

void printRoomList(BSTNode *x, Session *s) {
    if (x == NULL) return;
    else {
        Message rmsg;
        serverMessage(&rmsg, MSG_ITEM, x->key);
        printRoomList(x->left, s);
        sendMessage(s->socket, &rmsg);
        printRoomList(x->right, s);
    }
}

int commandPrivate(Session *s, Message *msg) {
    Message rmsg;
    char *receiver = strtok(msg->text, " ");
    receiver++; //remove '@' of the string
    strncpy(msg->username, s->username, USERNAME_SIZE);
    Session* dst = findSession(_onlineUsers, receiver);
    if(!dst) {
        serverMessage(&rmsg, MSG_ERROR, "Username not found.");
        serverSendMessage(s, &rmsg);
        return -1;
    }
    char *text = strtok(NULL, "");
    strncpy(msg->text, text, TEXT_SIZE);
    sessionSendMessage(dst, msg);
    sessionSendMessage(s, msg);
    fprintf(stdout, "Client \"%s\" sent message \"%s\" to \"%s\".\n",
            s->username, msg->text, dst->username);
    return 0;
}

void serverReadMessage(Session *s, Message *msg) {
    if(sessionReadMessage(s, msg) <= 0) {
        commandLogout(s);
    }
}

void serverSendMessage(Session *s, Message *msg) {
    if(sessionSendMessage(s, msg) <= 0) {
        commandLogout(s);
    }
}

