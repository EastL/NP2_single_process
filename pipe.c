#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include "shell.h"
#include "command.h"
#include "token.h"
#include "pipe.h"

pipe_node *pipe_front = NULL;
pipe_node *pipe_rear = NULL;

pipe_node *err_pipe_front = NULL;
pipe_node *err_pipe_rear = NULL;

void push_pipe(pipe_node **node)
{
	if (pipe_front == NULL)
	{
		pipe_front = pipe_rear = *node;
	}

	else
	{
		pipe_rear->next = *node;
		pipe_rear = *node;
	}
}


pipe_node *check(int count)
{
	pipe_node *temp = pipe_front;
	pipe_node *ret = NULL;
	while (temp != NULL)
	{
		if (temp->num == count)
			ret = temp;

		temp = temp->next;
	}

	return ret;
}

void free_pipe(pipe_node *node)
{
	node->num = 0;
	node->infd = 0;
	node->outfd = 0;
	node->next = NULL;
	free(node);
}

void decress_count()
{
	pipe_node *temp = pipe_front;
	pipe_node *pre = NULL;

	while (temp != NULL)
	{
		if (--temp->num == -1)
		{
			//do unlink and free
			if (pre == NULL)
			{
				pipe_front = temp->next;
				free_pipe(temp);

				//for next
				temp = pipe_front;
			}
			else
			{
				pre->next = temp->next;
				free_pipe(temp);
				//for next
				temp = pre->next;
			}	
		}

		else
			temp = temp->next;

	}
}
