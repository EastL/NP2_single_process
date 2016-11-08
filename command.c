#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "util.h"
#include "token.h"
#include "command.h"

static cmd_node *cfront = NULL;
static cmd_node *crear = NULL;

void push_cmd(cmd_node **node)
{
	if (cfront == NULL)
		cfront = crear = *node;
	else
	{
		crear->next = *node;
		crear = *node;
	}
}

cmd_node *pull_cmd()
{
	if (cfront == NULL)
	{
		printf("ya!\n");
		return NULL;
	}
	cmd_node *temp = cfront;
	cfront = cfront->next;
	return temp;
}

void print_cmd()
{
	cmd_node *tmp = cfront;
	while (tmp != NULL)
	{
		printf("cmd:%s\n", tmp->cmd);
		printf("arg:%s\n", tmp->arg[0]);
		printf("in:%d\n", tmp->in);
		printf("out:%d\n", tmp->out);
		//printf("Next:%x\n", tmp->next);

		tmp = tmp->next;
	}
}

void free_cmd(cmd_node *node)
{
	free(node->cmd);
	node->cmd = NULL;
	free(node->arg);
	node->arg = NULL;
	node->arg_count = 0;
	node->pip_count = 0;
	node->in = 0;
	node->out = 0;
	node->is_pipe_n = 0;
	node->is_redir = 0;
	node->next = NULL;
	free(node);
}

void free_cmd_line()
{
	cmd_node *temp = NULL;
	while (cfront != NULL)
	{
		temp = pull_cmd();
		free_cmd(temp);
	}
	
	crear = NULL;
}
