#include <stdlib.h>
#include <string.h>

void split(char **arr, char *str, const char *del)
{
	char *s = strtok(str, del);

	while (s != NULL)
	{
		*arr++ = s;
		s = strtok(NULL, del);
	}
}
