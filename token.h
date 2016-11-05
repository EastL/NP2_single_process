#ifndef __TOKEN__
#define __TOKEN__

#define CMDF 0
#define PIPE 1
#define REDIR 2
#define PIPEN 3
#define NEWLINE 4

struct __token
{
	int token_type;
	char *token;
	struct __token *next;	
};
typedef struct __token token_node; 

void push_node(token_node **node);
token_node *pull_node();
void tokenizer(int cfd);
int get_token_type(char *str);
void push_enter_node();
void print_node();

#endif
