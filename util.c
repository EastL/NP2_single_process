#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>

void split(char ***arr, char *str, const char *del, int *count)
{
	int _count = 0;
	char **_arr = malloc(sizeof(char) * 5000);

	char *save;
	char *s = strtok_r(str, del, &save);

	while (s != NULL)
	{
		int __count = strlen(s) + 1;
		*(_arr + _count) = malloc(sizeof(char) * __count);
		strncpy(*(_arr + _count), s, __count);
		s = strtok_r(NULL, del, &save);
		_count++;
	}

	*count = _count;
	*arr = _arr;
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
