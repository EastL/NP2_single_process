#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include "shell.h"

void shell(int client_fd)
{
	char *welcome = "****************************************\n** Welcome to the information server. **\n****************************************\n% ";
	char *shellsign = "%";

	send(client_fd, welcome, strlen(welcome), 0);
}
