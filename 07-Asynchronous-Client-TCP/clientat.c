 /* 
 	File: 		clientat.c
	Version: 	Asynchronous TCP Client using select() 
	Modified by: 	Joe O'Regan

	Year 4 Networked Games Assignment

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690

	Joe O'Regan 17/11/2017
	(Reference strcliselect02.h P228 UNIX® Network Programming Volume 1, Third Edition)
	The Modified TCP Client that can handle input asynchronously from the socket and file descriptors using the `select()` function
*/

#include <stdio.h>										// printf()
#include <string.h>										// Warning for strcpy, bzero()
#include <stdlib.h>										// Warning for exit
#include <unistd.h>										// gethostname(), write(), read(), close()
#include "../DrawHangman.h"									// Functions to display graphics and colour
#include "../Hangman.h"										// Functions to play Hangman game
#include "../CreateTCPSocket.h"									// Functions to create TCP Sockets
	
int main(int argc, char **argv) {
	int sock, maxfdp1, stdineof, byteCount;							// Socket, number of file descriptors, select on standard input, number of bytes
	fd_set rset;										// List of descriptors
	char buf[LINESIZE];									// Buffer

	// Make the socket connection, with default localhost, or entered IP address
	struct sockaddr_in servaddr = createTCPClientSocket(&sock, 				// CreateTCPSocket.h: Create the connection between the client & server, 
			(argc == 1) ? SRV_IP : argv[1], 					// use default IP address of localhost if none entered
			(argc == 3) ? atoi(argv[2]) : TCP_PORT_NUM);				// If 3 parameters are entered, use the given port number, 
	stdineof = 0;										// Flag to select on standard input, each loop
	FD_ZERO(&rset);										// Clear the list of descriptors

	drawHangmanLogo(ASYNC_TCP_CLI);								// Display game logo
	
	byteCount = read (sock, buf, LINESIZE);							// Read 1st (empty) part word from server
	write (1, buf, byteCount);								// Display on screen

	for ( ; ; ) {										// Infinite Guess loop
		if (stdineof == 0) FD_SET(fileno(stdin), &rset);				// FD_SET - add stdin to file descriptor list
		FD_SET(sock, &rset);								// Add socket to file descriptor list. 
		maxfdp1 = max(fileno(stdin), sock) + 1;						// Maximum number of file descriptors to test
		select(maxfdp1, &rset, NULL, NULL, NULL);					// Get the minimum of the 2 descriptors using select() and use it

		if (FD_ISSET(sock, &rset)) {							// socket is readable, FD_ISSET = check socket is in file descriptor list
			if ((byteCount = read(sock, buf, LINESIZE)) == 0) {			// Get input from socket
				if (stdineof == 1) break;					// normal termination, EOF already received
				else displayErrMsg("Server Has Terminated the Connection");	// HandleErrors.h: read() returns 0 if the server terminates the connection
			}

			write(fileno(stdout), buf, byteCount);					// Write to screen
		}

		if (FD_ISSET(fileno(stdin), &rset)) {  						// input is readable, check standard input is in list of file descriptors
			if ((byteCount = read(fileno(stdin), buf, LINESIZE)) == 0) {		// Get input from keyboard, and if none
				stdineof = 1;							// EOF received, set stdineof flag, call shutdown, and send FIN
				shutdown(sock, SHUT_WR);					// send FIN, SHUT_WR = close write half of connection
				FD_CLR(fileno(stdin), &rset);					// Remove standard input from the file descriptors list
				continue;							// Skip the write to server socket, and loop again
			}

			write(sock, buf, byteCount);						// Otherwise, Send input to socket
		}
	}
	close(sock);										// Close the socket
	exit(0);										// Terminate the program, (terminating open descriptors)
}
