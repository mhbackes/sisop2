/*
 * client.c
 * Created on: 2016-04-19
 *     Author: mhbackes <mhbackes@inf.ufrgs.br>
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>

#include "message.h"
#include "room.h"

#define PORT 4000
#define BUFF_SIZE 128

pthread_t sender;
pthread_t receiver;

char username[USERNAME_SIZE + 1];

/* remove fgets \n */
void trimm(char *s) {
    int i = strlen(s) - 1;
    if(s[i] == '\n')
        s[i] = '\0';
}

void getUserInput(Message *msg) {
    // TODO better user input handling
    strcpy(msg->username, username);
    fgets(msg->text, TEXT_SIZE, stdin);
    trimm(msg->text);
    if(msg->text[0] == '/') {
        if(!strncmp("quit", &msg->text[1], 4))
           msg->type = MSG_LOGOUT; 
        else if(!strncmp("name ", &msg->text[1], 5)) {
           msg->type = MSG_NAME; 
           strncpy(msg->text, &msg->text[6], USERNAME_SIZE);
        } else if(!strncmp("create ", &msg->text[1], 7)) {
           msg->type = MSG_CREATE_ROOM; 
           strncpy(msg->text, &msg->text[8], ROOMNAME_SIZE);
        } else if(!strncmp("delete ", &msg->text[1], 7)) {
           msg->type = MSG_DELETE_ROOM; 
           strncpy(msg->text, &msg->text[8], ROOMNAME_SIZE);
        } else if(!strncmp("join ", &msg->text[1], 5)) {
           msg->type = MSG_JOIN_ROOM; 
           strncpy(msg->text, &msg->text[6], ROOMNAME_SIZE);
        } else if(!strncmp("leave", &msg->text[1], 6)) {
           msg->type = MSG_LEAVE_ROOM; 
        } else
            msg->type = MSG_CHAT;
    } else
        msg->type = MSG_CHAT;
}

void login(int socket) {
    Message msg;
    clientMessage(&msg, MSG_LOGIN, username, "");
    sendMessage(socket, &msg);
    readMessage(socket, &msg);
    if(msg.type == MSG_ERROR) {
        fprintf(stderr, "Error: %s\n", msg.text);
        close(socket);
        exit(-1);
    }
}

void *cliSnd(void *args) {
	int sockfd = *((int*) args);
    Message msg;
	do {
        getUserInput(&msg);
        if(sendMessage(sockfd, &msg) <= 0) {
            fprintf(stdout, "Server disconnected.\n");
            close(sockfd);
            exit(0);
        }
	} while (msg.type != MSG_LOGOUT);
    close(sockfd);
    exit(0);
	return NULL;
}

void *cliRcv(void *args) {
	int sockfd = *((int*) args);
    Message msg;
	while(1) {
        if(readMessage(sockfd, &msg) <= 0) {
            fprintf(stdout, "Server disconnected.\n");
            close(sockfd);
            exit(0);
        }
        switch(msg.type) {
            case MSG_SUCCESS:
            case MSG_CHAT:
		        fprintf(stdout, "%s: %s\n", msg.username, msg.text);
                break;
            case MSG_ERROR:
		        fprintf(stdout, "%s: ERROR - %s\n", msg.username, msg.text);
                break;
        }
	}
}

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;
	
    if (argc < 3) {
		fprintf(stderr,"usage %s hostname username\n", argv[0]);
		exit(0);
    }
	strncpy(username, argv[2], USERNAME_SIZE);

    server = gethostbyname(argv[1]);
	if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
        printf("ERROR opening socket\n");
    
	serv_addr.sin_family = AF_INET;     
	serv_addr.sin_port = htons(PORT);    
	serv_addr.sin_addr = *((struct in_addr *)server->h_addr_list[0]);
	bzero(&(serv_addr.sin_zero), 8);     
	
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        printf("ERROR connecting\n");

    login(sockfd);
	pthread_create(&sender, NULL, cliSnd, &sockfd);
	pthread_create(&receiver, NULL, cliRcv,&sockfd);

	pthread_join(sender, NULL);
    
	close(sockfd);
    return 0;
}

