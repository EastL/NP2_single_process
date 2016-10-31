#ifndef __COMMAND__
#define __COMMAND__

struct __cmd
{
	char *cmd;
	char *arg;
	int pip_count;
	struct __cmd *nextcmd;
};

typedef struct __cmd cmd_node;
#endif
