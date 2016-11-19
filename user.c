#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include "user.h"

void push_user(user_node **front, user_node **rear, user_node *node)
{
	if (*front == NULL)
	{
		*rear = NULL;
		*front = *rear = node;
	}

	else
	{
		(*rear)->next = node;
		*rear = node;
	}
}

void remove_user(user_node *node)
{
	free(node->name);
	node->name = NULL;
	node->user_fd = 0;
	node->user_pipe_front = NULL;
	node->user_pipe_rear = NULL;
	node->user_cmd_front = NULL;
	node->user_cmd_rear = NULL;
	node->next = NULL;
}

void unlink_user(user_node **front, user_node **rear, user_node *node)
{
	if (*front == node && *rear == node)
	{
		*front = NULL;
		*rear = NULL;
	}

	user_node *pre = NULL;
	user_node *cur = *front;

	while(cur != NULL)
	{
		if (cur == node)
		{
			if (pre == NULL)
			{
				//only node
				*front = NULL;
				*rear = NULL;
				remove_user(node);
			}

			else
			{
				if (*rear == node)
				{
					//last node
					pre->next = NULL;
					*rear = pre;
					remove_user(node);
				}
				
				else
				{
					pre->next = cur->next;
					remove_user(node);
				}
			}

			cur = NULL;
		}

		else
			cur = cur->next;
	}
}

/*
void broadcast_message(const char *m)
{
	
}
*/
