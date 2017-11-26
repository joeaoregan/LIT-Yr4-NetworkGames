 /* 
 	File: 		server.c
	Version: 	Select Server
	Author:		Joe O'Regan

	Year 4 Networked Games Assignment

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690

	08/11/2017 	Added struct for holding session state for each client
			Client had been causing problems with buffers overwriting
*/

#include <stdio.h>				// printf(), snprintf()
#include <string.h>				// 23/09/2017 Warning for strcpy, bzero()
#include <stdlib.h>				// 23/09/2017 exit(), rand(), srand()
#include <unistd.h>				// 23/09/2017 gethostname(), write(), read(), close()
#include <time.h>				// time()
#include "../DrawHangman.h"			// Display the hangman graphics
#include "../Hangman.h"				// 13/10/2017 Hangman functions
#include "../CreateTCPSocket.h"			// Create a TCP Server Socket
#include "../Socket.h"				// Functions common to TCP and UDP versions
#include "../TCPPlayHangman.h"			// Functions to play Hangman with TCP connection
#include "../GameState.h"			// Functions to control the game state

struct sockaddr_in cliaddr;			// IPv4 Address structure for Client socket

int main(int argc, char **argv) {	
	struct 		sessionData client[FD_SETSIZE];					// GameState.h: game state structure
	int		i, j, maxi, maxfd, listenfd, connfd, sockfd, nready;
	ssize_t		n;
	fd_set		rset, allset;
	socklen_t	clilen;								// Size of the Client address structure
// Fix
	listenfd = createTCPServerSocket(TCP_PORT);					// CreateTCPSocket.h: Create the socket

	drawHangmanLogo(SELECT_SERVER);							// DrawHangman.h: Draw Hangman Game Logo Graphic, with label set with enum parameter
	
	srand(time(NULL));								// Random word each time the server starts

	maxfd = listenfd;								// initialize
	maxi = -1;									// index into client[] array
	for (i = 0; i < FD_SETSIZE; i++) client[i].sock = -1;				// -1 indicates available entry
	FD_ZERO(&allset);								// Initialise the set
	FD_SET(listenfd, &allset);							// Add listenfd to the set

/* MAIN LOOP */
    for ( ; ; ) {
		/*
			select() watches the list of file descriptors, listening for incoming connections, without blocking
			Can keep track of which sockets are ready to read, or if there are any errors on connected sockets
		*/
		rset = allset;								// structure assignment
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);			// select() - used to monitor multiple file descriptors

/* CLIENT CONNECT */
		if (FD_ISSET(listenfd, &rset)) {					// new client connection
			clilen = sizeof(cliaddr);					// Size of the client address
			connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &clilen);// Accept a connection on connfd socket
			
			// Add client to clients array
			for (i = 0; i < FD_SETSIZE; i++) {										
				if (client[i].sock < 0) {				// If the array position is empty
					client[i].sock = connfd;	
/* CLIENT DETAILS */			// Store and display the client IP and Port 
					displayIPAndPort((struct sockaddr_storage* )&cliaddr, clilen, // CreateTCPSocket.h: Display the Client IP and Port number
						client[i].ip, &client[i].sock, &client[i].port);

/*========================================== initialise the variables for each client ==========================================*/
					initClient(&client[i]);									// GameState.h: Init word, part word, lives, and game state for Client
					displayState(i,client[i].word,client[i].partWord,client[i].lives,client[i].gameState);	// GameState.h: Display the current game state

					sendPartWord(client[i].sock, client[i].bufOut, client[i].partWord, client[i].lives);	// TCPPlayHangman: send the part word string to client
					break;
				}
			}
			if (i == FD_SETSIZE) displayErrMsg("too many clients");							// Too many Client connections, display error message & exit game

			FD_SET(connfd, &allset);										/* add new descriptor to set */
			if (connfd > maxfd) maxfd = connfd;									/* for select */
			if (i > maxi) maxi = i;											/* max index in client[] array */

			if (--nready <= 0) continue;										/* no more readable descriptors */
		}

/* FOR EACH CLIENT */
		for (i = 0; i <= maxi; i++) {											/* check all clients for data */
			if ((sockfd = client[i].sock) < 0) continue;								// Handle the next client in the array

			if (FD_ISSET(sockfd, &rset)) {										// Check the socket is in the descripter set (fd_set)
			  	if (client[i].gameState == 'I') {								// If the current clients state is 'I', play the game
/* READ CLIENT GUESS: Process guess or close */
					if ((n = read(sockfd, client[i].bufIn, LINESIZE)) == 0) {				// Get the guess from the current client and if the client has finished
						close(sockfd);									// Close the connection to the client
						FD_CLR(sockfd, &allset);							// Remove the socket from the set
						
						closeSocketConnection(&client[i]);						// Reset Client and Client State
					} else {										// A guess has been received
/*==============================================================================================================================*/
/*  						        Hangman Code		     						*/
/*==============================================================================================================================*/
						client[i].bufIn[1]='\0';							// Only need 1 letter, so this will cut all the gibberish
/* CHECK GUESS */				checkGuess(client[i].bufOut, client[i].word,client[i].partWord, client[i].bufIn,// TCPPlayHangman.h: check the input from the client, format & display on server, 
									&client[i].lives, client[i].ip, client[i].port);	// and set the number of lives/guesses remaining

/* SHOW WORD */					if ((client[i].gameState = checkGameState(client[i].word, client[i].partWord, 
							client[i].lives)) == 'L') 						// Hangman.h: Check the win/lose/continue state of the game
							strcpy (client[i].partWord, client[i].word);				// Show the player the word if they lose

/* WRITE BACK TO CLIENT*/			sendPartWord(sockfd, client[i].bufOut, client[i].partWord, client[i].lives);	// Hangman.h: Send part word & number of guesses left to the client as a string

/* WIN MSG ONLY */				if (checkGameOver(client[i].gameState,client[i].bufOut,client[i].word, sockfd, 
						    client[i].ip, client[i].port)) {						// Hangman.h: XXX Write
							close(sockfd);								// Close the connection
							FD_CLR(sockfd, &allset);						// Remove the socket from the set
							closeSocketConnection(&client[i]);					// If game is finished, display win/lose message
						}
					}
				}												// CHANGED POSITION end when game state not = I
				if (--nready <= 0) break;
			}
		}
	}
}

