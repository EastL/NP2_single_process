#ifndef __SHELL__
#define __SHELL__
#include "command.h"

void shell(int client_fd);
int execute_node(cmd_node *node, int client_fd);
int check_cmd_exist(char *cmd, char *path);
char *trick(char *str);
#endif
