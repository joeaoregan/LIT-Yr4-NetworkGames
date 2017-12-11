all : cli srv

#CC specifies which compiler we're using
CC = gcc

cli : clientua.c
	$(CC) -o cli clientua.c

srv : serverua.c
	$(CC) -o srv serverua.c
