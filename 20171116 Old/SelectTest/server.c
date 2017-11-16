#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>											// printf()
#include <errno.h>
#include <string.h>											// 23/09/2017 Warning for strcpy, bzero()
#include <stdlib.h>											// 23/09/2017 Warning for exit
#include <unistd.h>											// 23/09/2017 gethostname(), wgccrite(), read(), close()
#include "../Hangman.h"											// Hangman functions
#include "../DrawHangman.h"										// Display the hangman graphics
#include "../CreateTCPServer.h"										// Create a TCP Server Socket

#define	LISTENQ		1024										/* 2nd argument to listen() */
char* SERV_PORT = "9877";										/* TCP and UDP */

char clntName[INET_ADDRSTRLEN];										// Client address string
struct sockaddr_in	cliaddr, servaddr;
#define CLI_PORT 	ntohs(cliaddr.sin_port)


struct sessionData {				// Store the game state, maybe use a linked list of structs instead of array
	int sock;				// The client socket that is connected
	char* word;				// Store the current full word
	char partWord[LINESIZE];		// Store the current part word
	int lives;				// Store the number of guesses left for each client
	int gameState;				// Store the game state for the connection
	char bufIn[LINESIZE];			// BUFFER
	char bufOut[LINESIZE];			// BUFFER
	char ip[INET_ADDRSTRLEN];		// client IP
	int port;				// client port
};

void show(char* msg);

int main(int argc, char **argv) {
	struct 			sessionData client[FD_SETSIZE];	
	int 			i, j, maxi, maxfd, listenfd, connfd, sockfd, nready;
	ssize_t			n;
	fd_set			rset, allset;
	socklen_t		clilen;

	listenfd = createTCPServerSocket(SERV_PORT);							// Create the socket

	maxfd = listenfd;										/* initialize */
	maxi = -1;											/* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++) client[i].sock = -1;						/* -1 indicates available entry */
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	for ( ; ; ) {
		rset = allset;										/* structure assignment */
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(listenfd, &rset)) {							/* new client connection */
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
			
			for (i = 0; i < FD_SETSIZE; i++) {
				if (client[i].sock < 0) {
					client[i].sock = connfd;					/* save descriptor */
/**/					// Store and display the client IP and Port 
					if (inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, client[i].ip,sizeof(client[i].ip)) != NULL){	// inet_ntop() - Convert IP address to human readable form
  						printf("Handling client %s/%d \n", client[i].ip, CLI_PORT);				// Display the client IP address and port number
						client[i].port = CLI_PORT;
					}
					client[i].word = selectRandomWord(client[i].ip, client[i].port);
					for (j = 0; j < strlen(client[i].word); j++) client[i].partWord[j]='-';				// Display hyphens to indicate size of word				 	
					client[i].partWord[j] = '\0';		
					client[i].lives = MAX_LIVES;
					client[i].gameState = 'I';
					printf("\nState %d Word: %s partWord: %s lives %d gameState %c\n\n", 
						i,client[i].word,client[i].partWord,client[i].lives,client[i].gameState);

					sprintf (client[i].bufOut, "%s %d \n", client[i].partWord, client[i].lives);			// Set outbuf to the part word & the lives left
 					write (connfd, client[i].bufOut, LINESIZE);							// Send to client
/**/
					break;
				}
			}
			if (i == FD_SETSIZE) printf("too many clients");								// err_quit("too many clients");

			FD_SET(connfd, &allset);											/* add new descriptor to set */
			if (connfd > maxfd) maxfd = connfd;										/* for select */
			if (i > maxi) maxi = i;												/* max index in client[] array */

			if (--nready <= 0) continue;											/* no more readable descriptors */
		}

		for (i = 0; i <= maxi; i++) {												/* check all clients for data */
			if ( (sockfd = client[i].sock) < 0) continue;
			if (FD_ISSET(sockfd, &rset)) {
				if ( (n = read(sockfd, client[i].bufIn, LINESIZE)) == 0) {						/* connection closed by client */
					close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i].sock = -1;										// Reset the client
				} else {
					write (1, client[i].bufIn, n);									// Display the data on server

					if (!correctGuess(client[i].word, client[i].partWord, client[i].bufIn)) client[i].lives--;
					if ((client[i].gameState = checkGameState(client[i].word, client[i].partWord, client[i].lives)) == 'L') strcpy (client[i].partWord, client[i].word);// Show the player the word if they lose
					sprintf(client[i].bufOut, "%s %d \n", client[i].partWord, client[i].lives);			// Add the part-word and number of guesses left to the buffer string
					write(sockfd, client[i].bufOut, strlen(client[i].bufOut));					// Send data to the client
					if (gameOverSelect(client[i].gameState,client[i].bufOut,client[i].word, sockfd, clntName, CLI_PORT)) close(sockfd);	
				}

				if (--nready <= 0) break;										/* no more readable descriptors */
			}
		}
	}
}

void show(char* msg){
	printf("\nMessage Sent: %s", msg);
}
