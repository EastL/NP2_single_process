#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <string.h>
#include <unistd.h>
#include "shell.h"
#include "command.h"
#include "parse.h"
#include "util.h"
#include "pipe.h"
#include "user.h"

int shell(user_node *client_fd)
{
	char *shellsign = "% ";
	char *line = NULL;
	ssize_t bufsize = 0;
	cmd_node *cmd_list;

	//set env
	for (int c = 0; c < client_fd->env_num; c++)
		setenv(client_fd->env[c], client_fd->envval[c], 1);

	line = malloc(sizeof(char) * 10010);


	//read and parsing line
	parse(client_fd);
	print_cmd(&(client_fd->user_cmd_front));

	//execute process
	cmd_node *current_cmd = pull_cmd(&(client_fd->user_cmd_front), &(client_fd->user_cmd_rear));

	int next_pipe_num = 0;

	while (current_cmd != NULL)
	{
		if (strncmp(current_cmd->cmd, "setenv", 6) == 0)
		{
			if (current_cmd->arg[1] == NULL || current_cmd->arg[2] == NULL)
			{
				char *ts = "Please give args.\n";
				write(client_fd->user_fd, ts, strlen(ts));
				current_cmd = pull_cmd(&(client_fd->user_cmd_front), &(client_fd->user_cmd_rear));
				continue;
			}

			//write to user info
			int cmp = 0;
			for (int c = 0; c < client_fd->env_num; c++)
			{
				if (strcmp(client_fd->env[c], current_cmd->arg[1]) == 0)
				{
					cmp = 1;
					strcpy(client_fd->envval[c], current_cmd->arg[2]);
				}
			}

			if (!cmp)
			{
				strcpy(client_fd->env[client_fd->env_num], current_cmd->arg[1]);
				strcpy(client_fd->envval[client_fd->env_num], current_cmd->arg[2]);
				client_fd->env_num++;
			}

			//real set
			setenv(current_cmd->arg[1], current_cmd->arg[2], 1);
			free_cmd(current_cmd);

			//last command, decress
			decress_count(&(client_fd->user_pipe_front), &(client_fd->user_pipe_rear));

			current_cmd = pull_cmd(&(client_fd->user_cmd_front), &(client_fd->user_cmd_rear));
		}

		else if (strncmp(current_cmd->cmd, "printenv", 8) == 0)
		{
			if (current_cmd->arg[1] == NULL)
			{
				char *ts = "Please give args.\n";
				write(client_fd->user_fd, ts, strlen(ts));
				fflush(stdout);
				current_cmd = pull_cmd(&(client_fd->user_cmd_front), &(client_fd->user_cmd_rear));
				continue;
			}

			//char *env_name = malloc(strlen(current_cmd->arg[1]) - 1);
			//strncpy(env_name, current_cmd->arg[1], (strlen(current_cmd->arg[1]) - 1));
			char *env_val = getenv(current_cmd->arg[1]);//sooooock
			char *ret = malloc(strlen(env_val) + strlen(current_cmd->arg[1]) + 4);
			sprintf(ret, "%s=%s\n", current_cmd->arg[1], env_val);
			
			write(client_fd->user_fd, ret, strlen(ret));
			free_cmd(current_cmd);

			//last command, decress
			decress_count(&(client_fd->user_pipe_front), &(client_fd->user_pipe_rear));

			current_cmd = pull_cmd(&(client_fd->user_cmd_front), &(client_fd->user_cmd_rear));
		}

		else if (strncmp(current_cmd->cmd, "removeenv", 9) == 0)
		{
			if (current_cmd->arg[1] == NULL)
			{
				char *ts = "Please give args.\n";
				write(client_fd->user_fd, ts, strlen(ts));
				current_cmd = pull_cmd(&(client_fd->user_cmd_front), &(client_fd->user_cmd_rear));
				continue;
			}

			setenv(current_cmd->arg[1], "", 1);
			free_cmd(current_cmd);

			//last command, decress
			decress_count(&(client_fd->user_pipe_front), &(client_fd->user_pipe_rear));

			current_cmd = pull_cmd(&(client_fd->user_cmd_front), &(client_fd->user_cmd_rear));
		}
		
		else if (strncmp(current_cmd->cmd, "exit", 4) == 0)
		{
			return -1;
		}

		else
		{
			char *envpath = getenv("PATH");
			if (check_cmd_exist(current_cmd->cmd, envpath) == -1)
			{
				char *unkown = "Unknown command: [";
				char *untail = "].\n";
				write(client_fd->user_fd, unkown, strlen(unkown));
				write(client_fd->user_fd, current_cmd->cmd, strlen(current_cmd->cmd));
				write(client_fd->user_fd, untail, strlen(untail));

				//last command, decress
				decress_count(&(client_fd->user_pipe_front), &(client_fd->user_pipe_rear));

				free_cmd(current_cmd);
				free_cmd_line(&(client_fd->user_cmd_front), &(client_fd->user_cmd_rear));
				current_cmd = pull_cmd(&(client_fd->user_cmd_front), &(client_fd->user_cmd_rear));
				continue;
			}	

			else
			{
				//printf("next:%s\n", current_cmd->next->cmd);
				execute_node(current_cmd, client_fd, &next_pipe_num);
				free_cmd(current_cmd);
				current_cmd = pull_cmd(&(client_fd->user_cmd_front), &(client_fd->user_cmd_rear));
			}
		}
	}

	write(client_fd->user_fd, shellsign, strlen(shellsign));
	return 0;
}

int execute_node(cmd_node *node, user_node *client_fd, int *next_n)
{
	int stdinfd = -1;
	int stdoutfd = -1;
	int stderrfd = -1;
	int pipe_n = 0;
	int pipe_err = 0;

	if (node->is_init)
	{
		pipe_node *ch_node = NULL;
		ch_node = check(&(client_fd->user_pipe_front), 0);
		if (ch_node != NULL)
		{
			stdinfd = ch_node->infd;
			close(ch_node->outfd);
		}
/*
		pipe_node *cherr_node = NULL;
		cherr_node = check(0, 1);
		if (cherr_node != NULL)
		{
			stdinfd = cherr_node->infd;
			close(cherr_node->outfd);
		}
*/
	}

	else
		stdinfd = *next_n;

	if (node->type == ISPIPE)
	{
		int pip[2];
		pipe(pip);
		stdoutfd = pip[1];
		*next_n = pip[0];
	}

	else if (node->type == ISREDIR)
	{
		int filefd = open(node->file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IROTH);
		stdoutfd = filefd;
	}

	else if (node->type == ISPIPEN || node->type == ISPIPEERR)
	{
		pipe_node *pip_node = check(&(client_fd->user_pipe_front), node->pip_count);
		printf("count:%d\n", node->pip_count);

		if (pip_node == NULL)
		{
			pip_node = malloc(sizeof(pipe_node));
			pip_node->num = node->pip_count;

			//construct pipe
			int pipn[2];
			pipe(pipn);
			pip_node->infd = pipn[0];
			pip_node->outfd = pipn[1];
			pip_node->next = NULL;
			push_pipe(&(client_fd->user_pipe_front), &(client_fd->user_pipe_rear), &pip_node);
			printf("no find pipe infd:%d\n", pip_node->infd);
			printf("no find pipe outfd:%d\n", pip_node->outfd);
		}

		else
		{
			printf("find pipe infd:%d\n", pip_node->infd);
			printf("find pipe outfd:%d\n", pip_node->outfd);
		}
		
		stdoutfd = pip_node->outfd;
		pipe_n = 1;

		if (node->type == ISPIPEERR)
		{
			stderrfd = pip_node->outfd;
			pipe_err = 1;
		}
	}

	

	printf("cmd:%s\n", node->cmd);
	printf("type:%d\n", node->type);
	printf("infd:%d\n", stdinfd);
	printf("outfd:%d\n", stdoutfd);
	printf("errfd:%d\n", stderrfd);

	//last command, decress
	if (node->is_new)
		decress_count(&(client_fd->user_pipe_front), &(client_fd->user_pipe_rear));

	int pid = fork();

	if (pid == 0)
	{
		close(0);
		close(1);
		close(2);
		dup(client_fd->user_fd);
		dup(client_fd->user_fd);
		dup(client_fd->user_fd);
		close(client_fd->user_fd);

		if (stdinfd != -1)
		{
			close(0);
			dup(stdinfd);
			close(stdinfd);
		}

		if (stdoutfd != -1)
		{
			close(1);
			dup(stdoutfd);
			if (stderrfd == -1)
				close(stdoutfd);
		}

		if (stderrfd != -1)
		{
			close(2);
			dup(stderrfd);
			close(stderrfd);
		}

		execvp(node->cmd, node->arg);
	}

	else
	{

		if (stdinfd != -1)
		{
			close(stdinfd);
			printf("closed:%d\n", stdinfd);
		}

		if (stdoutfd != -1)
			if (!pipe_n)
			{
				close(stdoutfd);
				printf("closed:%d\n", stdoutfd);
			}


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
