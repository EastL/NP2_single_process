#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include "shell.h"
#include "command.h"
#include "token.h"

void parse(int sfd, cmd_node *cmd_list)
{
	//printf("parsing...\n");
	
	tokenizer(sfd);
	return;
}
