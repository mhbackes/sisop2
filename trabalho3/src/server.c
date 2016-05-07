/*
 * server.c
 * Created on: 2016-04-19
 *     Author: mhbackes <mhbackes@inf.ufrgs.br>
 */
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "message.h"
#include "room.h"
#include "session.h"
#include "sessionlist.h"

#define PORT 4000
#define BUFF_SIZE 128

SessionList *onlineUsers;
Room* room;

void *sessionThread(void *args) {
    Session *s = (Session*) args;
    Message msg;
    while(1) {
        readMessage(s->socket, &msg);
        roomBroadcast(room, &msg);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    //TODO init server correctly
    onlineUsers = createSessionList();
    room = createRoom("A room");
    //TODO init server correctly

	int sockfd, newsockfd;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
        printf("ERROR opening socket");
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(serv_addr.sin_zero), 8);     
    
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		printf("ERROR on binding");
	
	listen(sockfd, 5);
	
	clilen = sizeof(struct sockaddr_in);
	while(1) {
		if ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) == -1) 
			printf("ERROR on accept");

        Message msg;
        readMessage(newsockfd, &msg);
        Session *s = createSession(newsockfd, msg.username);
        if(insertSession(onlineUsers, s)) {
            printf("Username already taken.\n");
            deleteSession(s);
        } else {
            insertUser(room, s); // TODO multiple rooms
            sessionRun(s, sessionThread);
        }
	}

	close(sockfd);
	return 0; 
}
