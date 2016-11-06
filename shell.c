#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/wait.h>
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

	//welcome msg
	write(client_fd, welcome, strlen(welcome));

	line = malloc(sizeof(char) * 10010);

	do
	{
		write(client_fd, shellsign, strlen(shellsign));

		//read and parsing line
		parse(client_fd);
		print_cmd();

		//execute process
		cmd_node *current_cmd = pull_cmd();

		while (current_cmd != NULL)
		{
			if (strncmp(current_cmd->cmd, "setenv", 6) == 0)
			{
				if (current_cmd->arg[0] == NULL || current_cmd->arg[1] == NULL)
				{
					char *ts = "Please give args.\n";
					write(client_fd, ts, strlen(ts));
					current_cmd = pull_cmd();
					continue;
				}

				setenv(current_cmd->arg[0], current_cmd->arg[1], 1);
				free_cmd(current_cmd);
				current_cmd = pull_cmd();
			}

			else if (strncmp(current_cmd->cmd, "printenv", 8) == 0)
			{
				if (current_cmd->arg[0] == NULL)
				{
					char *ts = "Please give args.\n";
					write(client_fd, ts, strlen(ts));
					current_cmd = pull_cmd();
					continue;
				}

				printf("%lu\n", strlen(current_cmd->arg[0]));
				char *env_name = malloc(strlen(current_cmd->arg[0]) - 1);
				strncpy(env_name, current_cmd->arg[0], (strlen(current_cmd->arg[0]) - 1));
				char *env_val = getenv(env_name);
				
				char *ret = malloc(strlen(env_val) + strlen(current_cmd->arg[0]) + 4);
				sprintf(ret, "%s=%s\n", env_name, env_val);
				ret[strlen(ret)] = '\0';
				write(client_fd, ret, strlen(ret));
				free_cmd(current_cmd);
				current_cmd = pull_cmd();
			}
			
			else if (strncmp(current_cmd->cmd, "exit", 4) == 0)
			{
				return;
			}

			else
			{
				;	
			}
		}

	} while(1);
	
}
