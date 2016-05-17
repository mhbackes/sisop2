/*
 * message.c
 * Created on: 2016-05-07
 *     Alunos: Augusto, Marcos e Jean
 */

#include "message.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <string.h>
#include <strings.h>
#include <netinet/in.h>

int readMessage(int socket, Message *msg) {
    unsigned n = 0;
    int r = 0;
    char *buff = (char*) msg;
    bzero(buff, sizeof(Message));
    do {
        r += read(socket, &buff[n], sizeof(Message) - n);
        if(r <= 0) return r;
        n += r;
    } while(n < sizeof(Message));
    msg->type = ntohs(msg->type);
    return n;
}

int sendMessage(int socket, Message *msg) {
    unsigned n = 0;
    int r = 0;
    msg->type = htons(msg->type);
    char *buff = (char*) msg;
    do {
        r += write(socket, &buff[n], sizeof(Message) - n);
        if(r <= 0) return r;
        n += r;
    } while(n < sizeof(Message));
    msg->type = ntohs(msg->type);
    return n;
}

void serverMessage(Message* msg, int type, char* text) {
    msg->type = type;
    strncpy(msg->username, "SERVER", USERNAME_SIZE);
    strncpy(msg->text, text, TEXT_SIZE);
}

void clientMessage(Message* msg, int type, char* username, char* text) {
    msg->type = type;
    strncpy(msg->username, username, USERNAME_SIZE);
    strncpy(msg->text, text, TEXT_SIZE);
}
