 /* 
	Client for hangman select server

 	File: tcpselcli.c

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690
*/

//#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>					// printf()
//#include <syslog.h>
//#include <signal.h>
#include <errno.h>
#include <string.h>					// 23/09/2017 Warning for strcpy, bzero()
#include <stdlib.h>					// 23/09/2017 Warning for exit
#include <unistd.h>					// 23/09/2017 gethostname(), write(), read(), close()
#include <arpa/inet.h>					// 23/09/2017 inet_ntop()
#include "../Hangman.h"					// 11/10/2017 Hangamen header file
#include "../DrawHangman.h"				// 11/10/2017 Draw Hangman Graphic

void str_echo(int sockfd);
void str_cli(FILE *fp, int sockfd);
static ssize_t my_read(int fd, char *ptr);
ssize_t readline(int fd, void *vptr, size_t maxlen);

// Parse server data to these variables
char arg1PartWord[20];
int arg2LivesLeft;
int count;

int main(int argc, char **argv)
{
	int			sockfd;
	struct sockaddr_in	servaddr;

	if (argc != 2)								// If there is not 2 arguments entered
		//err_quit("usage: tcpcli <IPaddress>");
		printf("usage: tcpcli <IPaddress>");				// Display error message

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(HANGMAN_TCP_PORT);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);			// XXX

	connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));	// XXX

	drawHangman();								// Draw hangman Graphic
	printf("Client Connecting On Port: %d \n", ntohs(servaddr.sin_port));	// Display port number

	str_cli(stdin, sockfd);							/* do it all */

	exit(0);								// Terminate the program, and close TCP socket
}

/*
	P228 (6.7) UNP book, str_cli function, using select and shutdown
*/
void str_cli(FILE *fp, int sockfd)
{
	char sendline[LINESIZE], recvline[LINESIZE];

/*============= Server sending 1st, not normal, might have to change ===========*/
/*============= display first hangman graphic before entering guess ============*/
	if (readline(sockfd, recvline, LINESIZE) == 0)				// If no data received from server
		//err_quit("str_cli: server terminated prematurely");
		printf("str_cli: server terminated prematurely");		// Display error message
	
		//printf("\ntest1\n");
		sscanf(recvline, "%s %d", &(*arg1PartWord), &arg2LivesLeft);	// Separate the part word and guesses 
		//printf("\ntest2\n");
		//fputs(recvline, stdout);
		amputate(arg2LivesLeft);					// Display hangman graphic & guesses left
		fputs(arg1PartWord, stdout);					// Display the part word without the guesses
		//printf("\ntest3\n");
		printf("\nEnter Your Guess:\t");
		//printf("\ntest4\n");
/*==============================================================================*/

	while (fgets(sendline, LINESIZE, fp) != NULL) {				// Get character guess input from the player

		write(sockfd, sendline, strlen(sendline));			// Send the character entered to the server

		if (readline(sockfd, recvline, LINESIZE) == 0)			// If no data received
			//err_quit("str_cli: server terminated prematurely");
			printf("str_cli: server terminated prematurely");	// Display error message

		sscanf(recvline, "%s %d", &(*arg1PartWord), &arg2LivesLeft);	// Separate the part word and guesses

		amputate(arg2LivesLeft);					// Display hangman graphic & guesses left

		//fputs(recvline, stdout);					// Display the part word and the guesses
		fputs(arg1PartWord, stdout);					// Display the part word without the guesses
		//printf("\nEnter Your Guess:\t");
		fputs("\n", stdout);
	}

	while ((count = read (sockfd, sendline, LINESIZE)) > 0) {
		sscanf(recvline, "%s %d", &(*arg1PartWord), &arg2LivesLeft);	// Parse string data received from server into separate part-word and score variables
 		amputate(arg2LivesLeft);					// Display graphical represenation of lives left
		printf("%s\n", arg1PartWord);
	    	write (1, recvline, count);					// Display the data string received from the server on screen
 	    	count = read (0, sendline, LINESIZE);				// 0 = STDIN
 	    	write (sockfd, sendline, count);				// Send client input to server
 	}

	printf("Game Over!\n");		
}

/*
	my_read() and readline() from lib folder in unpv13e
*/
static int read_cnt;
static char *read_ptr;
static char read_buf[LINESIZE];

static ssize_t my_read(int fd, char *ptr) {
	if (read_cnt <= 0) {
again:
		if ( (read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
			if (errno == EINTR)
				goto again;
			return(-1);
		} else if (read_cnt == 0)
			return(0);
		read_ptr = read_buf;
	}

	read_cnt--;
	*ptr = *read_ptr++;
	return(1);
}

ssize_t readline(int fd, void *vptr, size_t maxlen) {
	ssize_t	n, rc;
	char	c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++) {
		if ( (rc = my_read(fd, &c)) == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;		/* newline is stored, like fgets() */
		} else if (rc == 0) {
			*ptr = 0;
			return(n - 1);		/* EOF, n - 1 bytes were read */
		} else
			return(-1);		/* error, errno set by read() */
	}

	*ptr = 0;				/* null terminate like fgets() */
	return(n);
}
