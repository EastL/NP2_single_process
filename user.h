#ifndef __USER__
#define __USER__
#include "command.h"
#include "pipe.h"

struct _user
{
	char *name;
	int user_fd;
	char env[256][1024];
	char envval[256][1024];

	pipe_node *user_pipe_front;
	pipe_node *user_pipe_rear;

	cmd_node *user_cmd_front;
	cmd_node *user_cmd_rear;

	struct _user *next;
};

typedef _user user_node;

#endif
