//#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>											// printf()
//#include <syslog.h>
//#include <signal.h>
#include <errno.h>
#include <string.h>											// 23/09/2017 Warning for strcpy, bzero()
#include <stdlib.h>											// 23/09/2017 Warning for exit
#include <unistd.h>											// 23/09/2017 gethostname(), write(), read(), close()
#include <arpa/inet.h>										// 23/09/2017 inet_ntop()
//#include <sys/wait.h>										// 08/10/2017 wait()
#include <time.h>
#include "../Hangman.h"										// 13/10/2017 Hangman functions
#include "../DrawHangman.h"

#define	LISTENQ		1024									/* 2nd argument to listen() */

char clntName[INET_ADDRSTRLEN];								// Client address string
struct sockaddr_in	cliaddr, servaddr;						// Client address string
//struct sockaddr_in server, client;						// OK - Internet socket address structures
#define CLI_PORT ntohs(cliaddr.sin_port)

int main(int argc, char **argv)
{
	int					i, maxi, maxfd, listenfd, connfd, sockfd;
	int					nready, arrClient[FD_SETSIZE];
	ssize_t				n;
	fd_set				rset, allset;
	char				buf[LINESIZE];
	socklen_t			clilen;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(HANGMAN_TCP_PORT);

	bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	drawHangman();																// Draw the hangman graphic

	listen(listenfd, LISTENQ);
	
	printf("Server now running on port: %d \n", ntohs(servaddr.sin_port));		// Display port number
/*------------------------------------------------------------------------------*/

	char * whole_word, part_word[LINESIZE], guess[LINESIZE];
	char outbuf[LINESIZE];
  	int i2, j, word_length, lives = MAX_LIVES, game_state = 'I', good_guess;
	/* Arrays to hold state for each connected client */
	char* arrWholeWords[FD_SETSIZE];											// whole_word for each client
	char* arrPartWords[FD_SETSIZE];												// part_word for each client
	int arrLives[FD_SETSIZE], arrStates[FD_SETSIZE];							// lives for each client, game_state for each client

	srand(time(NULL));															// Random word each time the server starts

/*------------------------------------------------------------------------------*/

	maxfd = listenfd;															/* initialize */
	maxi = -1;																	/* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++) arrClient[i] = -1;							/* -1 indicates available entry */
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

/* MAIN LOOP */
    for ( ; ; ) {
		rset = allset;															/* structure assignment */
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);
/* CLIENT CONNECT */
		if (FD_ISSET(listenfd, &rset)) {										/* new client connection */
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);

			// Display client details when it connects
			if (inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, clntName,sizeof(clntName)) != NULL){
  				printf("Handling client %s/%d \n", clntName, CLI_PORT);
			}
			
			// Add client to clients array
			for (i = 0; i < FD_SETSIZE; i++)
				if (arrClient[i] < 0) {
					arrClient[i] = connfd;										/* save descriptor */

/* CHOOSE WORD */
/*arr whole*/	 	arrWholeWords[i] = selectRandomWord(clntName, CLI_PORT);	// Select a random word from the list of words
					word_length = strlen(arrWholeWords[i]);						// Get length of word

					/* No letters are guessed Initially */
				 	for (j = 0; j < word_length; j++)
				 		part_word[j]='-';										// Display hyphens to indicate size of word
				 	
					part_word[j] = '\0';	
/*arr part*/		arrPartWords[i] = part_word;	
/*arr lives*/		arrLives[i] = MAX_LIVES;
/*arr state*/		arrStates[i] = 'I';
					printf("test part word: %s\n", arrPartWords[i]);

					break;
				}
			if (i == FD_SETSIZE) printf("too many clients");					// err_quit("too many clients");

			FD_SET(connfd, &allset);											/* add new descriptor to set */
			if (connfd > maxfd) maxfd = connfd;									/* for select */
			if (i > maxi) maxi = i;												/* max index in client[] array */

			if (--nready <= 0) continue;										/* no more readable descriptors */
		}

/* FOR EACH CLIENT */
		for (i = 0; i <= maxi; i++) {				/* check all clients for data */
			if ( (sockfd = arrClient[i]) < 0) continue;

			if (FD_ISSET(sockfd, &rset)) {
			 // while (arrStates[i] == 'I') {
			  if (arrStates[i] == 'I') {
/* READ CLIENT GUESS */
				if ( (n = read(sockfd, guess, LINESIZE)) == 0) {				// Get the guess from the client
					/* 4connection closed by client */
					close(sockfd);
					FD_CLR(sockfd, &allset);
					// Reset Client and Client State
					arrClient[i] = -1;											// Clear client from client index
					arrWholeWords[i] = "";										// whole_word for each client
					arrPartWords[i] = "";										// part_word for each client
					arrLives[i] = MAX_LIVES;									// lives for each client
					arrStates[i] = 'I';											// game_state for each client
				} else {
/*==============================================================================*/
/* 								Hangman Code									*/
/*==============================================================================*/
	    			write (1, guess, n);										// Display client guess on server side

/* GET FOR EACH CLIENT */
					if (!correctGuess(arrWholeWords[i], arrPartWords[i], guess))// If it's an incorrect guess,
						arrLives[i]--;											// decrement the number of lives

					if (strcmp (arrWholeWords[i], arrPartWords[i]) == 0)		// If all letters guessed correctly
 						arrStates[i] = 'W'; 
					else if (arrLives[i] <= 0) {								// If client has run out of guesses
			 			arrStates[i] = 'L'; 									/* L ==> User Lost */
			 			strcpy (arrPartWords[i], arrWholeWords[i]); 			/* User Show the word */
			 		}
			
/* WRITE BACK TO CLIENT*/	
    				//snprintf (buf, sizeof(buf), "%s %d \n", arrPartWords[i], arrLives[i]);
    				sprintf (buf, "%s %d \n", arrPartWords[i], arrLives[i]);
					//if (arrStates[i] == 'I') 
					write (sockfd, buf, strlen (buf));

					printf("part word: %s lives: %d state: %c\n", arrPartWords[i], arrLives[i], arrStates[i]);
/* FINISH GAME */

					//printf("buf: %s, sockfd: %ld, word: %s\n", outbuf, sizeof(sockfd), arrWholeWords[i]);
					checkGameOver(arrStates[i],outbuf,arrWholeWords[i], sockfd, clntName, CLI_PORT); // If game is finished, display win/lose message
/*					
					// Send game over message
					if (arrStates[i] == 'W') {
						printf("Client %s/%d has guessed correctly!\n", clntName, CLI_PORT);
			 			snprintf (outbuf, 80, "Player Has Guessed Correctly!\n");					// Set win message to return to client
			 			write (sockfd, outbuf, strlen (outbuf));	
					}
					else if (arrStates[i] == 'L') {
						printf("Client %s/%d is a loser!\n", clntName, CLI_PORT);
			 			snprintf (outbuf, 80, "Player Has Run Out Of Guesses!\n");					// Set lose message to return to client
			 			write (sockfd, outbuf, strlen (outbuf));	
					}
*/
					if (arrStates[i] == 'L' || arrStates[i] == 'W') close(sockfd);
				}

				if (--nready <= 0) break;														/* no more readable descriptors */
				}	// end while game state I
			}
		}
	}
}
