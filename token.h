#ifndef __TOKEN__
#define __TOKEN__

#define CMDF 0
#define PIPE 1
#define REDIR 2
#define NEWLINE 3
#define PIPEN 4

struct __token
{
	int token_type;
	char *token;
	struct __token *next;	
};

typedef __token token_node; 

void tokenizer();
int get_token_type(char *str);

#endif
