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

#define PORT 4000
#define BUFF_SIZE 128

pthread_t sender;
pthread_t receiver;

char username[USERNAME_SIZE + 1];

void getUserInput(Message *msg) {
    // TODO better user input handling
	printf("$ ");
    strcpy(msg->username, username);
    fgets(msg->text, TEXT_SIZE, stdin);
    if(msg->text[0] == '/') {
        if(strcmp("quit", &msg->text[1]))
           msg->type = MSG_LOGOUT; 
    } else
        msg->type = MSG_CHAT;
}

void *cliSnd(void *args) {
	int sockfd = *((int*) args);
    Message msg;
    msg.type = MSG_LOGIN;
    strcpy(msg.username, username);
    sendMessage(sockfd, &msg);
	do {
        getUserInput(&msg);
        sendMessage(sockfd, &msg);
	} while (msg.type != MSG_LOGOUT);
    exit(0);
	return NULL;
}

void *cliRcv(void *args) {
	int sockfd = *((int*) args);
    Message msg;
	while(1) {
        readMessage(sockfd, &msg);
		fprintf(stdout, "%s: %s", msg.username, msg.text);
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

	pthread_create(&sender, NULL, cliSnd, &sockfd);
	pthread_create(&receiver, NULL, cliRcv,&sockfd);

	pthread_join(sender, NULL);
    
	close(sockfd);
    return 0;
}

