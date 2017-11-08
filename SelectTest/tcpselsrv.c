 /* 
	Network server for hangman game

 	File: tcpselsrv.c

	Concurrent Hangman Server: handles multiple clients at a time using the select () function, keeping a set of clients
	and arrays to hold each clients state. And checking each client for input in turn, and processing the input on the server

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690
	
	15/10/2017	Added separate files to handle creating the socket, and displaying the address and port number
*/

//#include <sys/types.h>
#include <sys/socket.h>				// sockaddr, accept(), bind(), listen()
#include <netinet/in.h>				// sockaddr_in 
#include <stdio.h>				// printf()
//#include <syslog.h>
//#include <signal.h>
#include <errno.h>				// Defines errno int, set by system calls/library functions, to indicate an error
#include <string.h>				// 23/09/2017 Warning for strcpy, bzero()
#include <stdlib.h>				// 23/09/2017 Warning for exit
#include <unistd.h>				// 23/09/2017 gethostname(), write(), read(), close()
#include <arpa/inet.h>				// 23/09/2017 inet_ntop()
//#include <sys/wait.h>				// 08/10/2017 wait()
#include <time.h>
#include "../Hangman.h"				// 13/10/2017 Hangman functions
#include "../DrawHangman.h"			// Display the hangman graphics
#include "../CreateTCPServer.h"			// Create a TCP Server Socket

#define	LISTENQ		1024			/* 2nd argument to listen() */

char clntName[INET_ADDRSTRLEN];			// Client address string
struct sockaddr_in	cliaddr, servaddr;	// Client address string
//struct sockaddr_in server, client;		// OK - Internet socket address structures
#define CLI_PORT ntohs(cliaddr.sin_port)

// maybe use a linked list of structs
struct sessionData {				// Store the game state
	char* fullWord;				// Store the current full word
	char* partWord;				// Store the current part word
	int numLives;				// Store the number of guesses left
	int gameState;				// Store the game state for the connection
	int wordSize;				// ??? the size of the word
};

int main(int argc, char **argv) {
	
	//struct sessionData states[20];	// hold state for 20 connections 
	struct sessionData state[FD_SETSIZE];	

	int		i, maxi, maxfd, listenfd, connfd, sockfd;
	int		nready, arrClient[FD_SETSIZE];
	ssize_t		n;
	fd_set		rset, allset;
	char		buf[LINESIZE];
	socklen_t	clilen;

	listenfd = createTCPServerSocket(TCP_PORT);									// Create the socket

	drawHangman();													// Draw the hangman graphic
/*======================================================================================================================*/

	char * whole_word, part_word[LINESIZE], guess[LINESIZE];
	char outbuf[LINESIZE];
  	int i2, j, word_length, lives = MAX_LIVES, game_state = 'I', good_guess;
	/* Arrays to hold state for each connected client */
	char* arrWords[FD_SETSIZE];											// whole_word for each client
	char* arrPart[FD_SETSIZE];											// part_word for each client
	char arrBuf[FD_SETSIZE][LINESIZE];
	int length[FD_SETSIZE];
	
	for (i = 0; i < FD_SETSIZE; i++)
		arrPart[FD_SETSIZE] = "-----------";

	int arrLives[FD_SETSIZE], arrStates[FD_SETSIZE];								// lives for each client, game_state for each client

	srand(time(NULL));												// Random word each time the server starts

/*======================================================================================================================*/

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
/* CHOOSE WORD */
/*arr whole*/	 			arrWords[i] = selectRandomWord(clntName, CLI_PORT);				// Select a random word from the list of words
					//word_length = strlen(arrWords[i]);						// Get length of word
					length[i] = strlen(arrWords[i]);						// Get length of word

					/* No letters are guessed Initially */
					//for (j = 0; j < word_length; j++)
/*init part word*/			for (j = 0; j < length[i]; j++)
						part_word[j]='-';							// Display hyphens to indicate size of word
						//arrPart[i][j]='-';							// Display hyphens to indicate size of word
				 	
					part_word[j] = '\0';								// Terminate the part_word string
					//arrPart[i][j]='\0';								// Terminate the part_word string
/*arr part*/				arrPart[i] = part_word;	
/*arr lives*/				arrLives[i] = MAX_LIVES;
/*arr state*/				arrStates[i] = 'I';								// Set the current clients state
					printf("test part word: %s\n", arrPart[i]);

	

					state[i].fullWord = arrWords[i];
					state[i].partWord = arrPart[i];
					state[i].numLives = MAX_LIVES;
					state[i].gameState = 'I';
					state[i].wordSize = strlen(state[i].partWord);

					printf("\nState %d fullWord: %s partWord: %s numLives %d gameState %c wordSize %d\n\n", 
						i,state[i].fullWord,state[i].partWord,state[i].numLives,state[i].gameState,state[i].wordSize);





/*================================ Server sending 1st, not normal, might have to change ================================*/
/*================================ display first hangman graphic before entering guess =================================*/
/*send part word*/			sprintf (buf, "%s %d \n", part_word, arrLives[i]);				// Set outbuf to the part word & the lives left
 					write (connfd, buf, strlen(buf));						// Send to client

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
			if ( (sockfd = arrClient[i]) < 0) continue;							// Handle the next client in the array

			if (FD_ISSET(sockfd, &rset)) {									// Check the socket is in the descripter set (fd_set)
			 // while (arrStates[i] == 'I') {
			  if (arrStates[i] == 'I') {									// If the current clients state is 'I', play the game
/* READ CLIENT GUESS */
				if ( (n = read(sockfd, arrBuf[i], LINESIZE)) == 0) {					// Get the guess from the current client
					/* 4connection closed by client */
					close(sockfd);									// Close the connection to the client
					FD_CLR(sockfd, &allset);							// Remove the socket from the set
					// Reset Client and Client State
					arrClient[i] = -1;								// Clear client from client index
					arrWords[i] = "";								// whole_word for each client
					arrPart[i] = "";								// part_word for each client
					arrLives[i] = MAX_LIVES;							// lives for each client
					arrStates[i] = 'I';								// game_state for each client
					arrBuf[i][0] = '\0';
				} else {

/*======================================================================================================================*/
/*THIS IS WHERE IT IS BROKE				Hangman Code							*/
/*======================================================================================================================*/

	    				write (1, arrBuf[i], n);							// Display client guess on server side
/* GET FOR EACH CLIENT */
					//printf("part 0: %s part1 %s\n", arrPart[0], arrPart[1]);
					//printf("Client %d part word Before checking guess: %s\n", i, arrPart[i]);

					if (!correctGuess(arrWords[i], arrPart[i], arrBuf[i]))				// If it's an incorrect guess,
						arrLives[i]--;								// decrement the number of lives

					if((arrStates[i] = checkGameState(arrWords[i], arrPart[i], arrLives[i])) == 'L')
			 			strcpy (arrPart[i], arrWords[i]); 					// Show the player the word if they lose
/* WRITE BACK TO CLIENT*/	
    					sprintf (arrBuf[i], "%s %d \n", arrPart[i], arrLives[i]);			// Add the part-word and number of guesses left to the buffer string
					write (sockfd, arrBuf[i], strlen (arrBuf[i]));					// Send the string to the client





					printf("Client %d part word: %s full word: %s lives: %d state: %c buffer: %s\n", 
					i, arrPart[i], arrWords[i],arrLives[i], arrStates[i], arrBuf[i]);		// Test the current client state




/* FINISH GAME */

					//printf("buf: %s, sockfd: %ld, word: %s\n", outbuf, sizeof(sockfd), arrWords[i]);
					checkGameOver(arrStates[i],outbuf,arrWords[i], sockfd, clntName, CLI_PORT); 	// If game is finished, display win/lose message

					if (arrStates[i] == 'L' || arrStates[i] == 'W') close(sockfd);
				}

				if (--nready <= 0) break;								/* no more readable descriptors */
				}	// end while game state I
			}
		}
	}
}

/*
guess[0] = arrBuf[i][0];
int good_guess = 0;
 	for (int g = 0; g < strlen(arrWords[i]); g++) {
 		if (guess [0] == arrWords[i][g]) {
 			good_guess = 1;
 			arrPart[i][g] = arrWords[i][g];
 		}
 	}
 	if (!good_guess) arrLives[i]--;
*/

