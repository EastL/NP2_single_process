#ifndef __COMMAND__
#define __COMMAND__

struct __cmd
{
	char *cmd;
	char **arg;
	int arg_count;
	int pip_count;
	int in;
	int out;
	struct __cmd *next;
};

typedef struct __cmd cmd_node;

void push_cmd(cmd_node **node);
cmd_node *pull_cmd();
#endif
