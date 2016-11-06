#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include "shell.h"
#include "command.h"
#include "parse.h"
#include "util.h"

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
				if (current_cmd->arg[1] == NULL || current_cmd->arg[2] == NULL)
				{
					char *ts = "Please give args.\n";
					write(client_fd, ts, strlen(ts));
					current_cmd = pull_cmd();
					continue;
				}

				setenv(current_cmd->arg[1], current_cmd->arg[2], 1);
				free_cmd(current_cmd);
				current_cmd = pull_cmd();
			}

			else if (strncmp(current_cmd->cmd, "printenv", 8) == 0)
			{
				if (current_cmd->arg[1] == NULL)
				{
					char *ts = "Please give args.\n";
					write(client_fd, ts, strlen(ts));
					current_cmd = pull_cmd();
					continue;
				}

				char *env_name = malloc(strlen(current_cmd->arg[1]) - 1);
				strncpy(env_name, current_cmd->arg[1], (strlen(current_cmd->arg[1]) - 1));
				char *env_val = getenv(env_name);//sooooock
				char *ret = malloc(strlen(env_val) + strlen(current_cmd->arg[1]) + 4);
				sprintf(ret, "%s=%s\n", env_name, env_val);
				
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
				char *envpath = getenv("PATH");
				if (check_cmd_exist(current_cmd->cmd, envpath) == -1)
				{
					char *unkown = "Unknown command: [";
					char *untail = "].\n";
					write(client_fd, unkown, strlen(unkown));
					write(client_fd, current_cmd->cmd, (strlen(current_cmd->cmd)-1));
					write(client_fd, untail, strlen(untail));
					free_cmd(current_cmd);
					free_cmd_line();
					current_cmd = pull_cmd();
					continue;
				}	

				else
				{
					execute_node(current_cmd);
					current_cmd = pull_cmd();
				}
			}
		}

	} while(1);
	
}

int execute_node(cmd_node *node)
{
	int pid = fork();

	if (pid == 0)
	{
		if (node->in != 0)
		{
			close(0);
			dup(node->in);
		}

		if (node->out != 1)
		{
			close(1);
			dup(node->out);
		}
		execvp(node->cmd, node->arg);
		
	}
	return 0;
}

int check_cmd_exist(char *cmd, char *path)
{
	size_t pnum;
	char **parray;
	char *full_path = malloc(sizeof(char) * 512);
	struct stat s;

	split(&parray, path, ":", &pnum);

	int16_t count;
	for (count = 0; count < pnum; count++)
	{
		sprintf(full_path, "%s/%s", parray[count], cmd);
		char *real_path = malloc(sizeof(char)*(strlen(full_path)-1));
		strncpy(real_path, full_path, (strlen(full_path)-1));
		
		if (stat(real_path, &s) != -1) //hooly shiiiiit
			return 1;
	}

	return -1;
}
