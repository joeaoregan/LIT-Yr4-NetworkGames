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
#include "../Hangman.h"										// 13/10/2017 Hangman funcions

#define	LISTENQ		1024									/* 2nd argument to listen() */
//#define LINESIZE 80
//#define MAX_LIVES 12	
//char *word [] = {
//  #include "../words"
//};
#define NUM_OF_WORDS (sizeof (word) / sizeof (word [0]))
#define MAXLEN 80 											/* Maximum size in the world of Any string */
//#define HANGMAN_TCP_PORT 1066								// The port number


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
	char				buf[MAXLEN];
	socklen_t			clilen;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(HANGMAN_TCP_PORT);

	bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	listen(listenfd, LISTENQ);
	
	printf("Server now running on port: %d \n", ntohs(servaddr.sin_port));		// Display port number
    //printf("FD_SETSIZE %d\n", FD_SETSIZE);	// check value of FD_SETSIZE
/*------------------------------------------------------------------------------*/

	char * whole_word, part_word[LINESIZE], guess[LINESIZE], outbuf[LINESIZE];
  	int i2, j, word_length, lives = MAX_LIVES, game_state = 'I', good_guess;
	/* Arrays to hold state for each connected client */
	int clientIndex;
	char* arrWholeWords[FD_SETSIZE];	// whole_word for each client
	char* arrPartWords[FD_SETSIZE];		// part_word for each client
	int arrLives[FD_SETSIZE];			// lives for each client
	int arrStates[FD_SETSIZE];			// game_state for each client
	int arrLengths[FD_SETSIZE];			// length of whole_word for each client

	srand(time(NULL));					// Random word each time the server starts

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
				 	/* Pick a word at random from the list */
					//whole_word = "test"; 
					//whole_word = selectRandomWord(clntName, CLI_PORT);		// Select a random word from the list of words
				 	//word_length = strlen(whole_word);							// Get length of word

/*arr whole*/	 	arrWholeWords[i] = selectRandomWord(clntName, CLI_PORT);	// Select a random word from the list of words
					word_length = strlen(arrWholeWords[i]);						// Get length of word

					/* No letters are guessed Initially */
				 	for (j = 0; j < word_length; j++)
				 		part_word[j]='-';										// Display hyphens to indicate size of word
				 	
					part_word[j] = '\0';	
/*arr part*/		arrPartWords[i] = part_word;	
/*arr lives*/		arrLives[i] = MAX_LIVES;
/*arr state*/		arrStates[i] = 'I';
/*arr length*/		arrLengths[i] = word_length;


					break;
				}
			if (i == FD_SETSIZE) printf("too many clients");					// err_quit("too many clients");

			FD_SET(connfd, &allset);											/* add new descriptor to set */
			if (connfd > maxfd) maxfd = connfd;									/* for select */
			if (i > maxi) maxi = i;												/* max index in client[] array */

			if (--nready <= 0) continue;										/* no more readable descriptors */
		}




/* FOR EACH CLIENT */
		for (clientIndex = 0; clientIndex <= maxi; clientIndex++) {				/* check all clients for data */
			if ( (sockfd = arrClient[clientIndex]) < 0) continue;
			if (FD_ISSET(sockfd, &rset)) {
/* READ CLIENT GUESS */
				if ( (n = read(sockfd, guess, MAXLEN)) == 0) {					// Get the guess from the client
					/* 4connection closed by client */
					close(sockfd);
					FD_CLR(sockfd, &allset);
					// Reset Client and Client State
					arrClient[clientIndex] = -1;				// Clear client from client index
					arrWholeWords[clientIndex] = "";			// whole_word for each client
					arrPartWords[clientIndex] = "";				// part_word for each client
					arrLives[clientIndex] = MAX_LIVES;			// lives for each client
					arrStates[clientIndex] = 'I';				// game_state for each client
					arrLengths[clientIndex] = 0;				// whole_word length for each client ????????????
				} else {
/*==============================================================================*/
/* 								Hangman Code									*/
/*==============================================================================*/
					good_guess = 0;

/*GET FOR EACH CLIENT*/
					//strcpy (part_word, arrPartWords[clientIndex]); 
					//whole_word = arrWholeWords[clientIndex];
					game_state = arrStates[clientIndex];

			 		for (i = 0; i < arrLengths[clientIndex]; i++) {
			 			//if (guess [0] == whole_word [i]) {						// If the guess from client is a match
			 			if (guess [0] == arrWholeWords[clientIndex][i]) {						// If the guess from client is a match
			 				good_guess = 1;										// Set good guess true
			 				//part_word [i] = whole_word [i];						// Fill the guessed letter in correct position
			 				arrPartWords[clientIndex] [i] = arrWholeWords[clientIndex][i];						// Fill the guessed letter in correct position
							//arrPartWords[clientIndex] = part_word; 							
			 			}
			 		}

					printf("part word: %s lives: %d\n", arrPartWords[clientIndex], arrLives[clientIndex]);

					if (! good_guess) arrLives[clientIndex]--;
					//if (strcmp (whole_word, part_word) == 0)							// If all letters guessed correctly
					if (strcmp (arrWholeWords[clientIndex], arrPartWords[clientIndex]) == 0)		// If all letters guessed correctly
 						game_state = 'W'; 
					//else if (lives == 0) {											// If client has run out of guesses
					else if (arrLives[clientIndex] == 0) {										// If client has run out of guesses
			 			game_state = 'L'; 												/* L ==> User Lost */
			 			strcpy (part_word, arrWholeWords[clientIndex]); 								/* User Show the word */
			 		}

/*SET FOR EACH CLIENT*/		
					//arrStates[clientIndex] = game_state;
			
/* WRITE BACK TO CLIENT*/					
	    			write (1, guess, n);												// Display client guess on server side

    				snprintf (buf, sizeof(buf), "%s %d \n", arrPartWords[clientIndex], arrLives[clientIndex]);
					write (sockfd, buf, strlen (buf));
				}
				if (--nready <= 0) break;												/* no more readable descriptors */
			}
		}
	}
}


