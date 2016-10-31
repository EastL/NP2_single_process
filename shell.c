#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include "shell.h"
#include "command.h"
#include "parse.h"

void shell(int client_fd)
{
	char *welcome = "****************************************\n** Welcome to the information server. **\n****************************************\n";
	char *shellsign = "% ";
	char *line = NULL;
	ssize_t bufsize = 0;
	cmd_node *cmd_list;

	if (client_fd < 0)
	{
		printf("Error, wrong fd:%d\n", client_fd);
		return;
	}

	//set env
	setenv("PATH", "bin:.", 1);

	printf("accept client %d\n", client_fd);

	//redirect stdin stdout stderr to socket
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	dup(client_fd);
	dup(client_fd);
	dup(client_fd);

	//welcome msg
	write(STDOUT_FILENO, welcome, strlen(welcome));

	line = malloc(sizeof(char) * 10010);

	do
	{
		write(STDOUT_FILENO, shellsign, strlen(shellsign));

		//read line
		int bufsize = read(STDIN_FILENO, line, 10000);

		//parsing line
		parse(cmd_list, line);

		//execute process
	} while(1);
}
