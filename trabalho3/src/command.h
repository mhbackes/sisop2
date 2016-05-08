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

void serverInit();
int startSession(int socket);
int execute(Session *s, Message *m);

#endif /* COMMAND_H */
