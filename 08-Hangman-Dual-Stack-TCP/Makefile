all : cli srv

#CC specifies which compiler we're using
CC = gcc

cli : clientds.c
	$(CC) -o cli clientds.c

srv : serverds.c
	$(CC) -o srv serverds.c
