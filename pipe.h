#ifndef __PIPE__
#define __PIPE__

struct __pipe
{
	int num;
	int infd;
	int outfd;
	struct __pipe *next;
};

typedef struct __pipe pipe_node;

void push_pipe(pipe_node **node);
pipe_node *check(int count);
#endif
