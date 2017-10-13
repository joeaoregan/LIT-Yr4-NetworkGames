//#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>										// printf()
#include <syslog.h>										// syslog()
//#include <signal.h>
#include <errno.h>
#include <string.h>										// 23/09/2017 Warning for strcpy, bzero()
#include <stdlib.h>										// 23/09/2017 Warning for exit
#include <unistd.h>										// 23/09/2017 gethostname(), write(), read(), close()
#include <arpa/inet.h>										// 23/09/2017 inet_ntop()
//#include <sys/wait.h>										// 08/10/2017 wait()
#include <time.h>										// 12/10/2017 srand() uses time
#include "Hangman.h"										// 11/10/2017 Hangamen header file
#include "DrawHangman.h"									// 11/10/2017 Draw Hanman Graphic

char clntName[INET_ADDRSTRLEN];									// Client address string
struct sockaddr_in server, client;								// OK - Internet socket address structures
#define CLI_PORT ntohs(client.sin_port)

int main(int argc, char **argv) {
	int				i, maxi, maxfd, sock, fd, sockfd, client_len;
	int				nready, clients[FD_SETSIZE];				// Renamed client[] to clients[] - array of integers for socket descriptors of each connected client
	ssize_t				n;
	fd_set				rset, allset;
	char				buf[MAXLINE];

	sock = socket(AF_INET, SOCK_STREAM, 0);							// OK

	bzero(&server, sizeof(server));								// OK
	server.sin_family      = AF_INET;							// OK
	server.sin_addr.s_addr = htonl(INADDR_ANY);						// OK
	server.sin_port        = htons(HANGMAN_TCP_PORT);					// OK

	bind(sock, (struct sockaddr *) &server, sizeof(server));				// OK

	listen(sock, MAX_CLIENTS);								// OK

	drawHangman();										// 11/10/2017 Moved Hangman Graphic To Header File
	
	printf("Server now running on port: %d \n", ntohs(server.sin_port));			// Display port number

// Array used to queue client connections

	maxfd = sock;										/* initialize */
	maxi = -1;										/* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++)
		clients[i] = -1;								/* -1 indicates available entry */


	FD_ZERO(&allset);
	FD_SET(sock, &allset);
/* end fig01 */

/* include fig02 */
	for ( ; ; ) {
		rset = allset;									/* structure assignment */
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(sock, &rset)) {							/* new client connection */
			client_len = sizeof(client);						// OK
			fd = accept(sock, (struct sockaddr *) &client, &client_len);		// OK

			if (inet_ntop(AF_INET, &client.sin_addr.s_addr, clntName,sizeof(clntName)) != NULL){
  				printf("Handling client %s/%d \n", clntName, CLI_PORT);
			}

// Add client to array
			for (i = 0; i < FD_SETSIZE; i++)
				if (clients[i] < 0) {
					clients[i] = fd;					/* save descriptor */
					break;
				}

			if (i == FD_SETSIZE) printf("too many clients"); 			//err_quit("too many clients");
		
			FD_SET(fd, &allset);							/* add new descriptor to set */
			if (fd > maxfd) maxfd = fd;						/* for select */
// Don't go past bounds of the array
			if (i > maxi) maxi = i;							/* max index in clients[] array */
// Decrement number of ready descriptors
			if (--nready <= 0) continue;						/* no more readable descriptors */
		}


		srand(time(NULL));

		for (i = 0; i <= maxi; i++) {							/* check all clients for data */
			if ( (sockfd = clients[i]) < 0) continue;
			//printf("Client: %d\n", i);

			if (FD_ISSET(sockfd, &rset)) {
				//printf("test\n");
// Play game
//				play_hangman(fd, fd);						// TEST


// Tidy up socket when done
				if ( (n = read(sockfd, buf, MAXLINE)) == 0) {			// 4connection closed by client /
// Close the socket, clear it from the set, Remove the socket from the array to free up array position for new connection
					close(sockfd);
					FD_CLR(sockfd, &allset);
					clients[i] = -1;
				} 

				else
					//write(sockfd, buf, n);
					//write(sockfd, "TEST TEST TEST", n);			// TEST
					//play_hangman(fd, fd);
					// Need to split play_hangman function???

				if (--nready <= 0) break;					/* no more readable descriptors */
			}
		}
	}
}


/* ---------------- Play_hangman () ---------------------*/

void play_hangman (int in, int out) {
  char * whole_word, part_word [LINESIZE],							// The word to guess, the word so far
  guess[LINESIZE], outbuf [LINESIZE];

  int i, word_length, lives = MAX_LIVES, game_state = 'I';					// Initialise the number of guesses, I = Incomplete
 
  displayHostname(out, outbuf);									// Display the name of the server
 
  whole_word = selectRandomWord(clntName, CLI_PORT);						// Select a random word from the list of words
  word_length = strlen(whole_word);								// Get length of word

  /* No letters are guessed Initially */
  for (i = 0; i < word_length; i++) part_word[i]='-';						// Display hyphens to indicate size of word 	
  part_word[i] = '\0';										// Add string terminating character

  snprintf (outbuf, sizeof(outbuf), "%s %d \n", part_word, lives);				// Convert to text string. Set outbuf to the part word & the lives left, with protection against buffer overflow
  write (out, outbuf, strlen(outbuf));								// Send to client





  while (game_state == 'I') {									// Loop until game is complete
    /* Get a letter from player guess */
    while (read (in, guess, LINESIZE) <0) {							// Read guess from client
      if (errno != EINTR) exit (4);								// Check for error
      printf ("re-read the startin \n");
    } 		
	
    if (!correctGuess(whole_word, part_word, guess)) lives--;					// If it's an incorrect guess, decrement the number of lives

    if (strcmp (whole_word, part_word) == 0) game_state = 'W'; 					// If all letters guessed correctly - W ==> User Won
    else if (lives == 0) {									// If client has run out of guesses
      game_state = 'L'; 									/* L ==> User Lost */
      strcpy (part_word, whole_word); 								/* User Show the word */
    }

    snprintf (outbuf, sizeof(outbuf), "%s %d \n", part_word, lives);				// Convert to text string. Set the part_word and lives to be sent, with protection against buffer overflow
    write (out, outbuf, strlen (outbuf));							// Send outbuf info to client
	
    checkGameOver(game_state, outbuf, sizeof(outbuf), whole_word, out, clntName, CLI_PORT); 	// If game is finished, display win/lose message
  }
}
