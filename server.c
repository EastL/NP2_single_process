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
	char *welcome = "****************************************\n** Welcome to the information server. **\n****************************************\n% ";
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

	//user info
	user_node *user_list_front = NULL;
	user_node *user_list_rear = NULL;

	//set select
	fd_set rfds;
	fd_set afds;
	int nfds = getdtablesize();

	FD_ZERO(&afds);
	FD_SET(my_fd, &afds);

	while(1)
	{
		printf("hhhhh\n");
		memcpy(&rfds, &afds, sizeof(rfds));
		if (select(nfds, &rfds, NULL, NULL, NULL) < 0)
			perror("select error");
		printf("select!!\n");

		if (FD_ISSET(my_fd, &rfds))
		{
			//accept client
			if ((clientfd = accept(my_fd, (struct sockaddr*)&client_socket, &cl_len)) < 0)
			{
				printf("Accept error!");
				continue;
			}

			printf("accept %d\n", clientfd);
			//add client info
			user_node *user = malloc(sizeof(user_node));
			memset(user, 0, sizeof(user_node));

			user->user_fd = clientfd;
			user->name = malloc(10);
			memset(user->name, 0, 10);
			user->name = "(no name)";
			
			push_user(&user_list_front, &user_list_rear, user);

			//welcome msg
			write(clientfd, welcome, strlen(welcome));
			printf("welcome\n");
			

			//broadcast
		
			//set afds
			FD_SET(clientfd, &afds);
		}

		else
		{
			printf("yoo\n");
			//find user for open shell
			user_node *active_user = user_list_front;
			while (active_user != NULL)
			{
				if (FD_ISSET(active_user->user_fd, &rfds))
					shell(active_user->user_fd);

				active_user = active_user->next;
			}
			
		}

	}

	return 0;
}
