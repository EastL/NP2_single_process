all:	
	gcc server.c shell.c command.c parse.c token.c util.c pipe.c -o rashell
	#gcc -g -fsanitize=address server.c shell.c command.c parse.c token.c util.c pipe.c -o rashell
