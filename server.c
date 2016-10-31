#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <unistd.h>
#include "shell.h"


int main()
{
	int sockfd;
	struct sockaddr_in mysocket;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&mysocket, sizeof(mysocket));
	mysocket.sin_family = AF_INET;
	mysocket.sin_addr.s_addr = INADDR_ANY;
	mysocket.sin_port = htons(3421);

	bind(sockfd, (struct sockaddr*) &mysocket, sizeof(mysocket));
	listen(sockfd, 20);

	while(1)
	{
		int clientfd;
		struct sockaddr_in client_socket;
		int cl_len = sizeof(client_socket);

		if ((clientfd = accept(sockfd, (struct sockaddr*)&client_socket, &cl_len)) < 0)
		{
			printf("Accept error!");
			continue;
		}

		int chpid = fork();

		if (chpid)
		{
			int status;
			int wpid = wait(&status);
			printf("wait pid %d, status %d\n", wpid, status);
		}
		
		else
		{
			shell(clientfd);
			close(clientfd);
			return 0;
		}
	}
}
