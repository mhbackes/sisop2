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
#include <ncurses.h>

#include "message.h"
#include "room.h"

#define PORT 4000

pthread_t sender, receiver;

WINDOW *receiverWindow, *senderWindow;
WINDOW *receiverBorder, *senderBorder;

char username[USERNAME_SIZE + 1];

void initUI();
void endUI();

void getUserInput(Message *msg) {
    // TODO better user input handling
    strcpy(msg->username, username);
    wgetnstr(senderWindow, msg->text, TEXT_SIZE);
    wclear(senderWindow);
    wrefresh(senderWindow);
    //trimm(msg->text);
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
	} else if(!strncmp("help", &msg->text[1], 5)) {
           msg->type = MSG_HELP; 
	} else if(!strncmp("clear", &msg->text[1], 6)) {
           msg->type = MSG_CLEAR; 
	} else if(!strncmp("ls", &msg->text[1], 3)) {
           msg->type = MSG_LS; 
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
        close(socket);
        endUI();
        fprintf(stderr, "Error: %s\n", msg.text);
		exit(0);
    }
}

void *cliSnd(void *args) {
	int sockfd = *((int*) args);
    Message msg;
	do {
        getUserInput(&msg);
        if(sendMessage(sockfd, &msg) <= 0) {
            close(sockfd);
            endUI();
            fprintf(stdout, "Server disconnected.\n");
			exit(0);
        }
	} while (msg.type != MSG_LOGOUT);
    close(sockfd);
    endUI();
	return NULL;
}

void *cliRcv(void *args) {
	int sockfd = *((int*) args);
    Message msg;
	while(1) {
        if(readMessage(sockfd, &msg) <= 0) {
            fprintf(stdout, "Server disconnected.\n");
            close(sockfd);
            endUI();
			exit(0);
        }
        int x, y;
        getyx(senderWindow, y, x); //saves current cursor position
        switch(msg.type) {
	case MSG_SUCCESS:
	case MSG_CHAT:
	  wprintw(receiverWindow, "%s: %s\n", msg.username, msg.text);
	  break;
	case MSG_ERROR:
	  wattron(receiverWindow, COLOR_PAIR(1));
	  wprintw(receiverWindow, "%s: ERROR - %s\n", msg.username, msg.text);
	  wprintw(receiverWindow, "\t Send \"/help\" to check the list of commands.\n");
	  wattroff(receiverWindow, COLOR_PAIR(1));
	  break;
	case MSG_HELP:
	  wattron(receiverWindow, COLOR_PAIR(5));
	  wprintw(receiverWindow, "%s: HELP - %s\n", msg.username, msg.text);
	  wattroff(receiverWindow, COLOR_PAIR(5));
	  break;
	case MSG_CLEAR:
	  wclear(receiverWindow);
	  break;
	case MSG_LS:
	  wprintw(receiverWindow, "%s: %s\n", msg.username, msg.text);
	  break;
	case MSG_ITEM:
	  wprintw(receiverWindow, "\t%s\n", msg.text);
	  break;
        }
        wrefresh(receiverWindow);
        wmove(senderWindow, y, x); // restores cursor position
        wrefresh(senderWindow);
    }
}

void initUI() {
    initscr();
    start_color();
    //    use_default_colors();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(5, COLOR_BLACK, COLOR_WHITE);
    cbreak();

    // init windows
    receiverWindow = newwin(LINES - 5, COLS - 2, 1, 1);
    senderWindow = newwin(2, COLS - 2, LINES - 3, 1); 
    receiverBorder = newwin(LINES - 3, COLS, 0, 0);
    senderBorder = newwin(4, COLS, LINES - 4, 0); 

    // set scrolling
    scrollok(receiverWindow, TRUE);

    // draw borders
    wborder(receiverBorder, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
                ACS_ULCORNER, ACS_URCORNER, ACS_LTEE, ACS_RTEE);
    wborder(senderBorder, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
                ACS_LTEE, ACS_RTEE, ACS_LLCORNER, ACS_LRCORNER);

    // refresh
    wrefresh(receiverBorder);
    wrefresh(senderBorder);
    wrefresh(receiverWindow);
    wrefresh(senderWindow);
}

void endUI() {
    endwin();
    delwin(senderWindow);
    delwin(senderBorder);
    delwin(receiverWindow);
    delwin(receiverBorder);
	setvbuf(stdout, NULL, _IOLBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
}

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;
	
    if (argc < 3) {
		fprintf(stderr,"usage %s hostname username\n", argv[0]);
		exit(-1);
    }
	strncpy(username, argv[2], USERNAME_SIZE);

    server = gethostbyname(argv[1]);
	if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(-1);
    }
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("ERROR opening socket\n");
        exit(-1);
    }
    
	serv_addr.sin_family = AF_INET;     
	serv_addr.sin_port = htons(PORT);    
	serv_addr.sin_addr = *((struct in_addr *)server->h_addr_list[0]);
	bzero(&(serv_addr.sin_zero), 8);     
	
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        printf("ERROR connecting\n");
        exit(-1);
    }

    initUI();
    login(sockfd);
	pthread_create(&sender, NULL, cliSnd, &sockfd);
	pthread_create(&receiver, NULL, cliRcv,&sockfd);

	pthread_join(sender, NULL);
    
	close(sockfd);
    return 0;
}

