#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <unistd.h>
#include <netdb.h>
#include "shell.h"
#include "user.h"

void wait4_child(int signo)
{
	int status;  
	while(waitpid(-1, &status, WNOHANG) > 0);  
}

int passivesock(const char *service, const char *transport, int qlen)
{
	struct servent  *pse;
	struct protoent *ppe;
	struct sockaddr_in sin;

	int s, type;

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;

	if ( pse = getservbyname(service, transport) )  
		sin.sin_port = htons(ntohs((unsigned short)pse->s_port));  

	else if ((sin.sin_port=htons((unsigned short)atoi(service))) == 0)  
		perror("can't get service entry\n");

	if ( (ppe = getprotobyname(transport)) == 0)
		perror("can't get protocol entry\n");

	if (strcmp(transport, "udp") == 0)
		type = SOCK_DGRAM;
	
	else
		type = SOCK_STREAM;

	
	s = socket(PF_INET, type, ppe->p_proto);

	int sock_opt = 1 ;
	if (setsockopt(s,SOL_SOCKET,SO_REUSEADDR,(void *)&sock_opt,sizeof(sock_opt)) == -1)
		perror("setsockopt error");

	if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		perror("bind error");

	if (type == SOCK_STREAM && listen(s, qlen) < 0)
		perror("listen error");
	
	return s;
}

int main()
{
	//kill zombie
	signal(SIGCHLD, wait4_child);

	//create my socket
	char port_service[5];
	sprintf(port_service, "%d", 13421);

	int my_fd = passivesock(port_service, "tcp", 5);

	//client socket
	int clientfd;
	struct sockaddr_in client_socket;
	int cl_len = sizeof(client_socket);

	//set select
	fd_set rfds;
	fd_set afds;
	int nfds = getdtablesize();

	FD_ZERO(&afds);
	FD_SET(my_fd, &afds);

	while(1)
	{
		memcpy(&rfds, &afds, sizeof(rfds));
		if (slelect(nfds, &rfds, NULL, NULL, NULL) < 0)
			perror("select error");

		if (FD_ISSET(my_fd, &rfds))
		{
			//accept client
			if ((clientfd = accept(sockfd, (struct sockaddr*)&client_socket, &cl_len)) < 0)
			{
				printf("Accept error!");
				continue;
			}

			//add client info
			user_node *user = malloc(sizeof(user_node));
			memset(user, 0, sizeof(use_node));

			user->user_fd = clientfd;
			user->name = malloc(10);
			memset(user->name, 0, 10);
			user->name = "(no name)";
			
			push_user(user);

			//broadcast
		
			//set afds
			FD_SET(clientfd, &afds);
		}

		else
		{
		}

		int chpid = fork();

		if (chpid == 0)
		{
			shell(clientfd);
			printf("bye~\n");
			exit(0);
		}
		
		else
		{
			int status;
			waitpid(chpid, &status, 0);
			close(clientfd);
		}
	}

	return 0;
}
