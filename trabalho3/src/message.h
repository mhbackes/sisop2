/*
 * message.h
 * Created on: 2016-05-07
 *	   Author: mhbackes <mhbackes@inf.ufrgs.br>
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>

#include "session.h"

#define TEXT_SIZE 511

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
    MSG_HELP,
    MSG_CLEAR,

    // server messages
    MSG_SUCCESS,
    MSG_ERROR
};

typedef struct message {
    int32_t type;
    char username[USERNAME_SIZE + 1];
    char text[TEXT_SIZE + 1];
} Message;

/*
 * Reads message msg from the given socket file descriptor.
 * Returns the number of bytes read on success and zero or less on fail.
 */
int readMessage(int socket, Message *msg);

/*
 * Writes message msg at the given socket file descriptor.
 * Returns the number of bytes written on success and zero or less on fail.
 */
int sendMessage(int socket, Message *msg);

/*
 * Initializes message msg as a server message.
 */
void serverMessage(Message* msg, int type, char* text);

/*
 * Initializes message msg as a client message.
 */
void clientMessage(Message* msg, int type, char* username, char* text);

#endif /* MESSAGE_H */
