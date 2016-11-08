#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include "shell.h"
#include "command.h"
#include "token.h"
#include "pipe.h"

void parse(int sfd)
{
	//printf("parsing...\n");
	token_node *last_node = NULL;
	token_node *current_node = get_node(sfd);

	if (current_node->token_type == NEWLINE)
	{
		free_token_node(current_node);
		free_token();
		return;
	}

	token_node *next_node = NULL;
	cmd_node *node;
	int16_t type = 0;
	int16_t last_type = 10;
	int16_t next_type = 10;
	
	do
	{
		type = current_node->token_type;
		next_node = get_node(sfd);
		next_type = next_node->token_type;
		if (last_node != NULL)
			last_type = last_node->token_type;
		//cmd or argv
		if (type == CMDF)
		{
			//cmd : first place or after pipe or after enter
			if (last_node == NULL || last_type == NEWLINE)
			{
				//new a cmd node
				node = malloc(sizeof(cmd_node));
				node->cmd = malloc(strlen(current_node->token)+1);
				strcpy(node->cmd, current_node->token);
				//node->cmd[strlen(current_node->token)] = '\0';
				node->arg = malloc(sizeof(char*) * 5);
				node->arg[0] = node->cmd;
				node->arg[1] = NULL;
				node->arg[2] = NULL;
				node->arg[3] = NULL;
				node->arg[4] = NULL;
				node->in = 0;
				node->out = 1;
				node->arg_count = 1;
				node->is_redir = 0;
				node->is_pipe_n = 0;
				node->next = NULL;

				//check stdin
				pipe_node *newnode = check(0);
				if (newnode != NULL)
				{
					node->in = newnode->outfd;
				}
			}

			else if (last_type == PIPE)
			{
				node->cmd = malloc(strlen(current_node->token)+1);
				strcpy(node->cmd, current_node->token);
				//node->cmd[strlen(current_node->token)] = '\0';
				node->arg = malloc(sizeof(char*) * 5);
				node->arg[0] = node->cmd;
				node->arg[1] = NULL;
				node->arg[2] = NULL;
				node->arg[3] = NULL;
				node->arg[4] = NULL;
				node->out = 1;
				node->arg_count = 1;
				node->is_redir = 0;
				node->is_pipe_n = 0;
				node->next = NULL;
			}
			
			else if (last_type == CMDF)
			{
				node->arg[node->arg_count] = malloc(strlen(current_node->token)+1);
				strcpy(node->arg[node->arg_count], current_node->token);
				//node->arg[node->arg_count][strlen(current_node->token)] = '\0';
				node->arg_count++;
			}

			if (next_type == NEWLINE)
			{
				push_cmd(&node);
			}
		}

		else if (type == PIPE)
		{
			//creat pipe
			int pip[2];
			pipe(pip);

			//old node
			node->out = pip[1];
			push_cmd(&node);
			node = malloc(sizeof(cmd_node));
			node->in = pip[0];
		} 
		
		else if (type == PIPEN)
		{
			pipe_node *get_same_count = check(current_node->token[1] - 0x30);			

			if (get_same_count == NULL)
			{
				//creat pipe
				int pip[2];
				pipe(pip);

				//old node
				node->out = pip[1];
				node->is_pipe_n = 1;
				push_cmd(&node);

				//pipe node
				pipe_node *new_node = malloc(sizeof(pipe_node));
				new_node->num = (current_node->token[1] - 0x30);
				new_node->infd = pip[0];
				new_node->outfd = pip[1];
				new_node->next = NULL;
				push_pipe(&new_node);
			}

			else
			{
				node->out = get_same_count->infd;
				push_cmd(&node);
			}
		}

		else if (type == REDIR)
		{
			;
		}
		
		if (last_node != NULL)
			free_token_node(last_node);
		last_node = current_node;
		current_node = next_node;

		if (next_type == NEWLINE)
		{
			free_token_node(last_node);
			free_token_node(current_node);
			break;
		}
	} while (1);

	free_token();
	return;
}
