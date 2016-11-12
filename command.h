#ifndef __COMMAND__
#define __COMMAND__

#define ISPIPE 1
#define ISPIPEN 2
#define ISPIPEERR 3
#define ISREDIR 4

struct __cmd
{
	char *cmd;
	char **arg;
	int arg_count;
	int pip_count;
	int type;
	char *file;
	int is_init;
	int is_new;
	struct __cmd *next;
};

typedef struct __cmd cmd_node;

void push_cmd(cmd_node **node);
cmd_node *pull_cmd();
void print_cmd();
void free_cmd(cmd_node *node);
void free_cmd_line();
#endif
