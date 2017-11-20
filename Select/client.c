/*	
	Joe O'Regan
	K00203642
	08/11/2017 	readline() not working correctly in loop, causing a delay in displaying the line received
			Changed to read() and now part word received from server displays correctly

			FIX: Displays final message twice
*/

#include <stdio.h>					// printf(), snprintf()
#include <errno.h>					// errno, EINTR
#include <string.h>					// 23/09/2017 Warning for strcpy, bzero()
#include <stdlib.h>					// 23/09/2017 exit(), rand()
#include <unistd.h>					// 23/09/2017 gethostname(), write(), read(), close()
#include <arpa/inet.h>					// 23/09/2017 inet_ntop()
#include "../Hangman.h"					// 11/10/2017 Hangamen header file
#include "../DrawHangman.h"				// 11/10/2017 Draw Hangman Graphic
#include "../CreateTCPSocket.h"
	
void str_echo(int sockfd);
static ssize_t my_read(int fd, char *ptr);
ssize_t readline(int fd, void *vptr, size_t maxlen);

// Parse server data to these variables
char arg1PartWord[20];
int arg2LivesLeft;
int count;

void getInput(int sock, char* recv);

int main(int argc, char **argv) {
	int sockfd;
	struct sockaddr_in servaddr;
	char sendline[LINESIZE], recvline[LINESIZE];

	servaddr = createTCPClientSocket(&sockfd, (argc == 1) ?  SRV_IP : argv[1]);		// Create socket and make connection

//	getInput(sockfd, recvline);

//	while (fgets(sendline, LINESIZE, stdin) != NULL) {					// Get input from keyboard	
 //	    	write(sockfd, sendline, strlen(sendline));					// Send client input to server		
	while (1) {						
//	while (read(sockfd, recvline, LINESIZE) != 0) {					
		if (read(sockfd, recvline, LINESIZE) == 0) break;				// Receive line from the server, break from loop when the connection is terminated
//		if (read(sockfd, recvline, LINESIZE) == 0) 					// Get input from Server
//			displayErrMsg("str_cli: server terminated prematurely");

		parseWordAndLives(recvline);							// Display the remaining guesses and part word

		// Get input from keyboard
		fgets(sendline, LINESIZE, stdin);
 	    	write(sockfd, sendline, strlen(sendline));					// Send client input to server
 	}

//	write(1,"Game Over!\n",LINESIZE);
	printf("%sGAME OVER!%s\n",RED,NORM);
//	count = read(sockfd, recvline, LINESIZE);				// Receive line from the server
	read(sockfd, recvline, LINESIZE);				// Receive line from the server
//	recvline[count] = '\0';
//	snprintf (recvline, strlen(recvline), "Player Has Run Out Of Guesses!\n");
//	printf("%s count: %d\n",recvline, count);
	write(0, recvline, strlen(recvline));
//	displayErrMsgStatus("Server terminated prematurely", 0);
	//exit(0);	
}
/*
void getInput(int sock, char* recv){
	if (read(sock, recv, LINESIZE) == 0) 							// Get input from Server
		displayErrMsg("str_cli: server terminated prematurely");

	sscanf(recv, "%s %d", &(*arg1PartWord), &arg2LivesLeft);				// Separate the part word and guesses
	selectLives(arg2LivesLeft);								// Display hangman graphic & guesses left
	fputs(arg1PartWord, stdout);								// Display the part word without the guesses
	fputs("\n", stdout);
}


static int	read_cnt;
static char	*read_ptr;
static char	read_buf[LINESIZE];

static ssize_t my_read(int fd, char *ptr) {
	if (read_cnt <= 0) {
again:												// Return to this point
		if ( (read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
			if (errno == EINTR) goto again;
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
			if (c == '\n') break;		// newline is stored, like fgets()
		} else if (rc == 0) {
			*ptr = 0;
			return(n - 1);			// EOF, n - 1 bytes were read
		} else
			return(-1);			// error, errno set by read()
	}

	*ptr = 0;					// null terminate like fgets()
	return(n);
}
*/
