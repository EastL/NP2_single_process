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

				//char *env_name = malloc(strlen(current_cmd->arg[1]) - 1);
				//strncpy(env_name, current_cmd->arg[1], (strlen(current_cmd->arg[1]) - 1));
				char *env_val = getenv(current_cmd->arg[1]);//sooooock
				char *ret = malloc(strlen(env_val) + strlen(current_cmd->arg[1]) + 4);
				sprintf(ret, "%s=%s\n", current_cmd->arg[1], env_val);
				
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
					//printf("next:%s\n", current_cmd->next->cmd);
					execute_node(current_cmd, client_fd);
					//free_cmd(current_cmd);
					current_cmd = pull_cmd();
				}
			}
		}

	} while(1);
	
}

int execute_node(cmd_node *node, int client_fd)
{
	int pid = fork();

	if (pid == 0)
	{

		close(0);
		close(1);
		close(2);
		dup(client_fd);
		dup(client_fd);
		dup(client_fd);

		if (node->in != 0)
		{
			printf("%d\n", node->in);
			close(0);
			dup(node->in);
			close(node->in);
		}

		if (node->out != 1)
		{
			printf("%d\n", node->out);
			close(1);
			dup(node->out);
			close(node->out);
		}

		//char *temp = malloc(sizeof(char)*(strlen(node->cmd)-1));
		//strncpy(temp, node->cmd, strlen(node->cmd)-1);
		execvp(node->cmd, node->arg);
	}

	else
	{
		int t;
		waitpid(pid, &t, 0);
	}
	return 0;
}

char *trick(char *str)
{
	char *ret = malloc(sizeof(char)*(strlen(str)-1));
	strncpy(ret, str, (strlen(str)-1));
	return ret;
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
		
		if (stat(full_path, &s) != -1) //hooly shiiiiit
			return 1;
	}

	return -1;
}
