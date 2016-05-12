/*
 * command.h
 * Created on: 2016-05-07
 *	   Author: mhbackes <mhbackes@inf.ufrgs.br>
 */

#ifndef COMMAND_H
#define COMMAND_H

#include "message.h"
#include "room.h"
#include "roomlist.h"
#include "session.h"
#include "sessionlist.h"

/*
 * Initializes server tree structures.
 */
void serverInit();

/*
 * Starts a new thread that will manage user requests received from the 
 * given socket. Returns 0 on success and -1 on error, when username
 * is already taken.
 */
int startSession(int socket);

/*
 * The session s executes the command request on message m. Returns 0 on
 * success and -1 on fail.
 */
int execute(Session *s, Message *m);

#endif /* COMMAND_H */
