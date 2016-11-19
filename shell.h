#ifndef __SHELL__
#define __SHELL__
#include "command.h"

int shell(int client_fd);
int execute_node(cmd_node *node, int client_fd, int *next_n);
int check_cmd_exist(char *cmd, char *path);
char *trick(char *str);
#endif
