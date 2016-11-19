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
		user_node *temp = *rear;
		temp->next = node;
		*rear = node;
	}
}

void remove_user(user_node *node)
{
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

		printf("cur:0x%x\n", cur);
		printf("cur->next:0x%x\n", cur->next);
		printf("pre:0x%x\n", pre);
		printf("node:0x%x\n", node);
		
		if (cur == node)
		{
			if (pre == NULL)
			{
				//first node
				*front = node->next;
			}

			else
			{
				if (*rear == node)
				{
					//last node
					pre->next = NULL;
					*rear = pre;
				}
				
				else
				{
					pre->next = cur->next;
				}
			}

			cur = NULL;
		}

		else
		{
			pre = cur;
			cur = cur->next;
		}
	}
}

/*
void broadcast_message(const char *m)
{
	
}
*/
