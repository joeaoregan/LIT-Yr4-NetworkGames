 /* 
 	File: 		client.c
	Version: 	Hangman Client using Select(), no read before loop, makes this different to other TCP client
	Author:		Joe O'Regan

	Year 4 Networked Games Assignment

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690

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
#include "../CreateTCPSocket.h"				// Functions to create and connect the socket
	
int count, win;

int main(int argc, char **argv) {
	int sockfd;
	struct sockaddr_in servaddr;
	char sendline[LINESIZE], recvline[LINESIZE];

	servaddr = createTCPClientSocket(&sockfd, (argc == 1) ?  SRV_IP : argv[1]);	// CreateTCPSocket.h: Create socket & make connection, using a default IP 127.0.0.1 if no paramaters, or command line argument if entered

	while (1) {		
		if (read(sockfd, recvline, LINESIZE) == 0) break;			// Receive line from the server, break from loop when the connection is terminated

		win = parseWordAndLives(recvline, 0);					// DrawHangman.h: Display the remaining guesses and part word, 0 = don't draw hangman, 1 = draw, while setting the game over text decision variable

		// Get input from keyboard
		fgets(sendline, LINESIZE, stdin);
 	    	write(sockfd, sendline, strlen(sendline));				// Send client input to server
 	}

	gameOverText(win);								// Display a game over message based on the number of lives left

	close(sockfd);									// Close the socket connection
	exit(0);									// Exit the application
}
