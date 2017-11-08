#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>									// printf()
#include <errno.h>
#include <string.h>									// 23/09/2017 Warning for strcpy, bzero()
#include <stdlib.h>									// 23/09/2017 Warning for exit
#include <unistd.h>									// 23/09/2017 gethostname(), write(), read(), close()
#include <arpa/inet.h>									// 23/09/2017 inet_ntop()

#define	LISTENQ		1024								/* 2nd argument to listen() */
# define LINESIZE 80
# define MAX_LIVES 12	
# define NUM_OF_WORDS (sizeof (word) / sizeof (word [0]))
# define MAXLEN 80 									/* Maximum size in the world of Any string */
# define HANGMAN_TCP_PORT 1066								// The port number

char clntName[INET_ADDRSTRLEN];								// Client address string
struct sockaddr_in	cliaddr, servaddr;						// Client address string
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
	
	printf("Server now running on port: %d \n", ntohs(servaddr.sin_port));						// Display port number
    //printf("FD_SETSIZE %d\n", FD_SETSIZE);	// check value of FD_SETSIZE
/*------------------------------------------------------------------------------*/

	char * whole_word, part_word[LINESIZE], guess[LINESIZE], outbuf[LINESIZE];
  	int i2, word_length, lives = MAX_LIVES, game_state = 'I', good_guess;
	/* Arrays to hold state for each connected client */
	int clientIndex;

 	/* Pick a word at random from the list */
	whole_word = "test";
 	word_length = strlen(whole_word);										// Get length of word

	/* No letters are guessed Initially */
 	for (i = 0; i < word_length; i++)
 		part_word[i]='-';											// Display hyphens to indicate size of word
 	
	part_word[i] = '\0';		

/*------------------------------------------------------------------------------*/

	maxfd = listenfd;												/* initialize */
	maxi = -1;													/* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++) arrClient[i] = -1;								/* -1 indicates available entry */
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	for ( ; ; ) {
		rset = allset;												/* structure assignment */
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(listenfd, &rset)) {									/* new client connection */
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);

			// Display client details when it connects
			if (inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, clntName,sizeof(clntName)) != NULL){
  				printf("Handling client %s/%d \n", clntName, CLI_PORT);
			}
			
			// Add client to clients array
			for (i = 0; i < FD_SETSIZE; i++)
				if (arrClient[i] < 0) {
					arrClient[i] = connfd;								/* save descriptor */
					break;
				}
			if (i == FD_SETSIZE) printf("too many clients");						// err_quit("too many clients");

			FD_SET(connfd, &allset);									/* add new descriptor to set */
			if (connfd > maxfd) maxfd = connfd;								/* for select */
			if (i > maxi) maxi = i;										/* max index in client[] array */

			if (--nready <= 0) continue;									/* no more readable descriptors */
		}

		for (clientIndex = 0; clientIndex <= maxi; clientIndex++) {						/* check all clients for data */
			if ( (sockfd = arrClient[clientIndex]) < 0) continue;
			if (FD_ISSET(sockfd, &rset)) {
/*READ*/
				if ( (n = read(sockfd, guess, MAXLEN)) == 0) {						// Get the guess from the client
					/* 4connection closed by client */
					close(sockfd);
					FD_CLR(sockfd, &allset);
					arrClient[clientIndex] = -1;							// Clear client from client index
				} else {
/*======================================================================================================================*/
/* 		   					Hangman Code							*/
/*======================================================================================================================*/
					good_guess = 0;
					
			 		for (i = 0; i < word_length; i++) {
			 			if (guess [0] == whole_word [i]) {					// If the guess from client is a match
			 				good_guess = 1;							// Set good guess true
			 				part_word [i] = whole_word [i];					// Fill the guessed letter in correct position
			 			}
			 		}
					if (! good_guess) lives--;
					if (strcmp (whole_word, part_word) == 0)					// If all letters guessed correctly
 						game_state = 'W'; 
/*WRITE*/					
	    				write (1, guess, n);								// Display client guess on server side

    					snprintf (buf, sizeof(buf), "%s %d \n", part_word, lives);
					write (sockfd, buf, strlen (buf));
				}
				if (--nready <= 0) break;								/* no more readable descriptors */
			}
		}
	}
}
