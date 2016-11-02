#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>

void split(char ***arr, char *str, const char *del)
{
	char *save;
	printf("hello\n");
	char *s = strtok_r(str, del, &save);

	while (s != NULL)
	{
		**arr++ = s;
		s = strtok_r(NULL, del, &save);
	}
}

int regular_match(char *str, char *reg_str)
{
	regex_t reg;
	int status;
	int cflags = REG_EXTENDED;
	regmatch_t pmatch[1];
	const size_t nmatch = 1;
	int err;

	if ((err = regcomp(&reg, reg_str, cflags)) != 0)
	{
		//char *err_buf;
		//regerror(err, &reg, err_buf);
		regfree(&reg);
		return -1;
	}

	if ((status = regexec(&reg, str, nmatch, pmatch, 0)) < 0)
	{
		regfree(&reg);
		return -1;
	}

	if (status == REG_NOMATCH)
		return 0;
	else
		return 1;	
}
