all : cli srv

#CC specifies which compiler we're using
CC = gcc

cli : hangclient.c
	$(CC) -o cli hangclient.c

srv : hangserver_iter.c
	$(CC) -o srv hangserver_iter.c
