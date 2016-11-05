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
	token_node *last_node = NULL;
	token_node *current_node = get_node(sfd);
	token_node *next_node = NULL;
	cmd_node *node;
	int16_t type = 0;
	int16_t last_type = 10;
	
	do
	{
		next_node = get_node(sfd);
		type = current_node->token_type;
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
				node->cmd = malloc(strlen(current_node->token));
				strncpy(node->cmd, current_node->token, strlen(current_node->token));
				printf("hello\n");
				node->arg = malloc(sizeof(char*) * 4);
				node->in = 0;
				node->out = 1;
				node->arg_count = 0;
			}

			else if (last_type == PIPE)
			{
				node->cmd = malloc(strlen(current_node->token));
				strncpy(node->cmd, current_node->token, strlen(current_node->token));
				node->arg = malloc(sizeof(char*) * 4);
				node->out = 1;
				node->arg_count = 0;
			}
			
			else if (last_type == CMDF)
			{
				node->arg[node->arg_count] = malloc(strlen(current_node->token));
				strncpy(node->arg[node->arg_count], current_node->token, strlen(current_node->token));
				node->arg_count++;
				
			}
		}

		else if (type == PIPE)
		{
			//creat pipe
			int pip[2];
			pipe(pip);

			//old node
			node->out = pip[0];
			node = malloc(sizeof(cmd_node));
			node->in = pip[1];
		} 
		
		last_node = current_node;
		current_node = next_node;
	} while (type != NEWLINE);
	return;
}
