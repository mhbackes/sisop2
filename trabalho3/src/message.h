/*
 * message.h
 * Created on: 2016-05-07
 *	   Author: mhbackes <mhbackes@inf.ufrgs.br>
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>

#include "session.h"

#define TEXT_SIZE 255

enum MessageType {
    // commands
    MSG_LOGIN, 
    MSG_LOGOUT, 
    MSG_CHAT, 
    MSG_NAME, 
    MSG_CREATE_ROOM,
    MSG_DELETE_ROOM,
    MSG_JOIN_ROOM, 
    MSG_LEAVE_ROOM, 

    // server messages
    MSG_SUCCESS,
    MSG_ERROR
};

typedef struct message {
    int32_t type;
    char username[USERNAME_SIZE + 1];
    char text[TEXT_SIZE + 1];
} Message;

int readMessage(int socket, Message *msg);
int sendMessage(int socket, Message *msg);

void serverMessage(Message* msg, int type, char* text);
void clientMessage(Message* msg, int type, char* username, char* text);

#endif /* MESSAGE_H */
