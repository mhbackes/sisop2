/*
 * message.c
 * Created on: 2016-05-07
 *     Author: mhbackes <mhbackes@inf.ufrgs.br>
 */

#include "message.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <strings.h>
#include <netinet/in.h>

int readMessage(int socket, Message *msg) {
    unsigned n = 0;
    int r = 0;
    char *buff = (char*) msg;
    bzero(buff, sizeof(Message));
    do {
        r += read(socket, &buff[n], sizeof(Message) - n);
        if(r < 0) {
            fprintf(stderr, "Error reading socket\n");
            exit(-1);
        }
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
        if(r < 0) {
            fprintf(stderr, "Error writing socket\n");
            exit(-1);
        }
        n += r;
    } while(n < sizeof(Message));
    return n;
}

