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
#include "../DrawHangman.h"				// 11/10/2017 Draw Hangman Graphic (Needs to be before Hangman.h define in both client and server for colour)
#include "../Hangman.h"					// 11/10/2017 Hangamen header file
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

	servaddr = createTCPClientSocket(&sockfd, (argc == 1) ?  SRV_IP : argv[1]);	// Create socket and make connection

	while (1) {		
		if (read(sockfd, recvline, LINESIZE) == 0) break;			// Receive line from the server, break from loop when the connection is terminated

		parseWordAndLives(recvline);						// Display the remaining guesses and part word

		// Get input from keyboard
		fgets(sendline, LINESIZE, stdin);
 	    	write(sockfd, sendline, strlen(sendline));				// Send client input to server
 	}

	read(sockfd, recvline, LINESIZE);						// Receive line from the server
	write(0, recvline, strlen(recvline));						// Display the line from Server on standard output

	printf("%sGAME OVER!%s\n",RED,NORM);
	//close(sockfd);									// Close the socket connection
	//exit(0);
}
