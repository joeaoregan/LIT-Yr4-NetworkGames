all : cli srv

#CC specifies which compiler we're using
CC = gcc

cli : clients.c
	$(CC) -o cli clients.c

srv : servers.c
	$(CC) -o srv servers.c
