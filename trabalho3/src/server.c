/*
 * server.c
 * Created on: 2016-04-19
 *     Alunos: Augusto, Marcos e Jean
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

#include "command.h"
#include "common.h"

int sockfd;
pthread_t commandLineThread;

void *serverCommandLine(void *args);

int main(int argc, char *argv[]) {
    // init
    serverInit();

	int sockfd, newsockfd;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("ERROR opening socket\n");
        exit(-1);
    }
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(serv_addr.sin_zero), 8);     
    
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		printf("ERROR on binding\n");
        exit(-1);
    }

	system("clear");
	system("echo \"======== SERVER ON : $(date) =========\"");
	listen(sockfd, 8);
	
	// server command line
	pthread_create(&commandLineThread, NULL, serverCommandLine, NULL);

	clilen = sizeof(struct sockaddr_in);
	while(1) {
		if ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) == -1) 
			printf("ERROR on accept\n");
        startSession(newsockfd);
	}

	return 0; 
}

void *serverCommandLine(void *args) {
	char buff[256] = {0};
	while (strncmp(buff, "quit", 4))
		fgets(buff, 256, stdin);
	close(sockfd);
	exit(0);
}
