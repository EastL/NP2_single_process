#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "util.h"
#include "token.h"

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

