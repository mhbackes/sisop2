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
    MSG_LOGIN, 
    MSG_LOGOUT, 
    MSG_CHAT, 
    MSG_NAME, 
    MSG_ENTER_ROOM, 
    MSG_EXIT_ROOM, 
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

#endif /* MESSAGE_H */
