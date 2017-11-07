#include <sys/socket.h>				// sockaddr, accept(), bind(), listen()
#include <netinet/in.h>				// sockaddr_in 
#include <stdio.h>				// printf()
#include <errno.h>				// Defines errno int, set by system calls/library functions, to indicate an error
#include <string.h>				// 23/09/2017 Warning for strcpy, bzero()
#include <stdlib.h>				// 23/09/2017 Warning for exit
#include <unistd.h>				// 23/09/2017 gethostname(), write(), read(), close()
#include <arpa/inet.h>				// 23/09/2017 inet_ntop()
#include <time.h>
#include "../Hangman.h"				// 13/10/2017 Hangman functions
#include "../DrawHangman.h"			// Display the hangman graphics
#include "../CreateTCPServer.h"			// Create a TCP Server Socket

char clntName[INET_ADDRSTRLEN];			// Client address string
struct sockaddr_in	cliaddr, servaddr;	// Client address string
#define CLI_PORT 	ntohs(cliaddr.sin_port)
#define	LISTENQ		1024			/* 2nd argument to listen() */

struct sessionData {				// Store the game state, maybe use a linked list of structs instead of array
	char* fullWord;				// Store the current full word
	char partWord[LINESIZE];				// Store the current part word
	int lives;				// Store the number of guesses left for each client
	int gameState;				// Store the game state for the connection
	char buf[LINESIZE];			// BUFFER
};

int main(int argc, char **argv) {	
	struct 		sessionData state[FD_SETSIZE];	
	int		i, j, maxi, maxfd, listenfd, connfd, sockfd, nready, arrClient[FD_SETSIZE];
	ssize_t		n;
	fd_set		rset, allset;
	char		outbuf[LINESIZE];
	socklen_t	clilen;

	listenfd = createTCPServerSocket(TCP_PORT);									// Create the socket

	drawHangman();													// Draw the hangman graphic
	
	srand(time(NULL));												// Random word each time the server starts

	maxfd = listenfd;												/* initialize */
	maxi = -1;													/* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++) arrClient[i] = -1;								/* -1 indicates available entry */
	FD_ZERO(&allset);												// Initialise the set
	FD_SET(listenfd, &allset);											// Add listenfd to the set

/* MAIN LOOP */
    for ( ; ; ) {
		rset = allset;												/* structure assignment */
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);							// select() - used to monitor multiple file descriptors
/* CLIENT CONNECT */
		if (FD_ISSET(listenfd, &rset)) {									/* new client connection */
			clilen = sizeof(cliaddr);									// Size of the client address
			connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);				// Accept a connection on connfd socket

			// Display client details when it connects
			if (inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, clntName,sizeof(clntName)) != NULL){		// inet_ntop() - Convert IP address to human readable form
  				printf("Handling client %s/%d \n", clntName, CLI_PORT);					// Display the client IP address and port number
			}
			
			// Add client to clients array
			for (i = 0; i < FD_SETSIZE; i++)										
				if (arrClient[i] < 0) {									// If the array position is empty
					arrClient[i] = connfd;								/* save descriptor */
/*====================================== initialise the variables for each client ======================================*/
/* CHOOSE WORD */ 			state[i].fullWord = selectRandomWord(clntName, CLI_PORT);			// Select a random word from the list of words
					/* No letters are guessed Initially */
/*init part word*/			for (j = 0; j < strlen(state[i].fullWord); j++) state[i].partWord[j]='-';		// Display hyphens to indicate size of word				 	
					state[i].partWord[j] = '\0';		
					state[i].lives = MAX_LIVES;
					state[i].gameState = 'I';

					printf("\nState %d fullWord: %s partWord: %s lives %d gameState %c\n\n", 
						i,state[i].fullWord,state[i].partWord,state[i].lives,state[i].gameState);

/*================================ Server sending 1st, not normal, might have to change ================================*/
/*================================ display first hangman graphic before entering guess =================================*/
/*send part word*/			sprintf (state[i].buf, "%s %d \n", state[i].partWord, state[i].lives);		// Set outbuf to the part word & the lives left
 					write (connfd, state[i].buf, LINESIZE);						// Send to client
/*======================================================================================================================*/
					break;
				}
			if (i == FD_SETSIZE) printf("too many clients");						// err_quit("too many clients");

			FD_SET(connfd, &allset);									/* add new descriptor to set */
			if (connfd > maxfd) maxfd = connfd;								/* for select */
			if (i > maxi) maxi = i;										/* max index in client[] array */

			if (--nready <= 0) continue;									/* no more readable descriptors */
		}
/* FOR EACH CLIENT */
		for (i = 0; i <= maxi; i++) {										/* check all clients for data */
			if ((sockfd = arrClient[i]) < 0) continue;							// Handle the next client in the array

			if (FD_ISSET(sockfd, &rset)) {									// Check the socket is in the descripter set (fd_set)
			  if (state[i].gameState == 'I') {								// If the current clients state is 'I', play the game
/* READ CLIENT GUESS */
				if ( (n = read(sockfd, state[i].buf, LINESIZE)) == 0) {					// Get the guess from the current client
					/* 4connection closed by client */
					close(sockfd);									// Close the connection to the client
					FD_CLR(sockfd, &allset);							// Remove the socket from the set
					// Reset Client and Client State
					arrClient[i] = -1;								// Clear client from client index
					state[i].fullWord = "";								// whole word for each client
					state[i].partWord[0] = '\0';								// part word for each client
					state[i].lives = MAX_LIVES;							// lives for each client
					state[i].gameState = 'I';							// game state for each client
					state[i].buf[0] = '\0';								// Clear client from client index
				} else {
/*THIS IS WHERE IT IS BROKE				Hangman Code							*/
/*======================================================================================================================*/
	    				write (1, state[i].buf, n);							// Display client guess on server side
/* GET FOR EACH CLIENT */
					if (!correctGuess(state[i].fullWord, state[i].partWord, state[i].buf))		// If it's an incorrect guess,
						state[i].lives--;							// decrement the number of lives

					if((state[i].gameState = checkGameState(state[i].fullWord, state[i].partWord, state[i].lives)) == 'L')
			 			strcpy (state[i].partWord, state[i].fullWord); 				// Show the player the word if they lose
/* WRITE BACK TO CLIENT*/	
    					sprintf (state[i].buf, "%s %d \n", state[i].partWord, state[i].lives);		// Add the part-word and number of guesses left to the buffer string
					write (sockfd, state[i].buf, strlen(state[i].buf));				// Send the string to the client
/*======================================================================================================================*/
					printf("Client %d part word: %s full word: %s lives: %d state: %c buf: %s\n", 
					i,state[i].partWord,state[i].fullWord,state[i].lives,state[i].gameState,state[i].buf);// Test the current client state
/* FINISH GAME */
/* WIN MSG ONLY */			checkGameOver(state[i].gameState,outbuf,state[i].fullWord, sockfd, clntName, CLI_PORT);// If game is finished, display win/lose message

					if (state[i].gameState == 'L' || state[i].gameState == 'W') close(sockfd);
				}

				if (--nready <= 0) break;								/* no more readable descriptors */
				}	// end while game state I
			}
		}
	}
}
