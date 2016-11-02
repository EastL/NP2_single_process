#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "../util.h"

int main()
{
	char *buf = "cat  ls  cd  aaa |4";
	char *del = " ";
	char **array;
	char *save;
	
	int res;

	char *t = strtok(buf, del);
	printf("h\n");
	char *s = strtok_r(buf, del, &save);
	printf("h\n");
	//split(&array, buf, del);
	//printf("%s\n", array[0]);
}
