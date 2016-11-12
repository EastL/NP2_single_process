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
		{
			ret = temp;
		}
	}

	return ret;
}
