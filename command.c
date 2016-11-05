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
		cfront->next = *node;
		crear = *node;
	}
}

cmd_node *pull_cmd()
{
	if (cfront == NULL)
		return NULL;

	cmd_node *temp = cfront;
	cfront = cfront->next;
	temp->next = NULL;
	return temp;
}
