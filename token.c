#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "util.h"
#include "token.h"

static token_node *front = NULL;
static token_node *rear = NULL;

int get_token_type(char *str)
{
	if (regular_match(str, "\\|[[:digit:]]+") == 1)
		return PIPEN;
	else if (regular_match(str, "\\|") == 1)
		return PIPE;
	else if (regular_match(str, ">") == 1)
		return REDIR;
	else if (regular_match(str, "\n") == 1)
		return NEWLINE;
	else 
		return CMDF;
}

void push_node(token_node **node)
{
	if (front == NULL && rear == NULL)
		front = rear  = *node;
	else
	{
		rear->next = *node;
		rear = *node;
	}
}

token_node *pull_node()
{
	if (front == NULL)
		return NULL;
	else
	{
		token_node *temp = front;
		front = front->next;
		return temp;
	}
}

void push_enter_node()
{
	token_node *temp = malloc(sizeof(token_node));
	temp->token = malloc(sizeof(int));
	temp->token[0] = '\n';
	temp->token[1] = '\0';
	temp->token_type = NEWLINE;
	temp->next = NULL;
	push_node(&temp);
}

void print_node()
{
	token_node *t = front;
	while(t != NULL)
	{
		printf("%s type:%d\n", t->token, t->token_type);
		t = t->next;
	}
}

void tokenizer(int cfd)
{
	char *buf = malloc(sizeof(char) * 10010);
	size_t token_n;
	char **token_array;
	size_t i;

	read(cfd, buf, 10010);

	//four situation
	
	split(&token_array, buf, " ", &token_n);

	for (i = 0; i < token_n; i++)
	{
		token_node *tnode = malloc(sizeof(token_node));
		if (strlen(token_array[i]) != 1 && token_array[i][strlen(token_array[i])-1] == '\n')
		{
			tnode->token = malloc(sizeof(char) * strlen(token_array[i]));
			token_array[i][strlen(token_array[i])-1] = '\0';
			strncpy(tnode->token, token_array[i], strlen(token_array[i]));
			tnode->token_type = get_token_type(token_array[i]);
			tnode->next = NULL;
			push_node(&tnode);
			push_enter_node();
		}
		
		else
		{
			tnode->token = malloc(sizeof(char) * (strlen(token_array[i]) + 1));
			strncpy(tnode->token, token_array[i], strlen(token_array[i]));
			tnode->token_type = get_token_type(token_array[i]);
			tnode->next = NULL;
			push_node(&tnode);
		}
			
	}

	//print_node();
}
