 /* 
	Network server for hangman game

 	File: hangserver.c

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690

	23/09/2017 	Added includes to get rid of compile warnings (wasn't necessary)
			Client IP and Port number is displayed when a connection is made
			Win/Lose message displayed, and sent to Client to display
			Added hangman graphic
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <syslog.h>
#include <signal.h>
#include <errno.h>
#include <string.h>											// 23/09/2017 Warning for strcpy
#include <stdlib.h>											// 23/09/2017 Warning for exit
#include <unistd.h>											// 23/09/2017 gethostname(), write(), read(), close()
#include <arpa/inet.h>											// 23/09/2017 inet_ntop()
#include "../DrawHangman.h"
#include "../Hangman.h"

extern time_t time ();			

char clntName[INET_ADDRSTRLEN];										// Client address string
struct sockaddr_in server, client;
#define CLI_PORT ntohs(client.sin_port)

void main ()												// No command line arguments
{
 	int sock, fd, client_len;
 	//struct sockaddr_in server, client;								// Made global to display client name outside main

 	srand ((int) time ((long *) 0)); 								/* randomize the seed */

 	sock = socket (AF_INET, SOCK_STREAM, 0);							// 0 or IPPROTO_TCP	// Create the socket
 	if (sock <0) { 											// This error checking is the code Stevens wraps in his Socket Function etc
 		perror ("creating stream socket");
 		exit (1);
 	}

 	server.sin_family = AF_INET;									// IPv4 address
 	server.sin_addr.s_addr = htonl(INADDR_ANY);							// Server IP
 	server.sin_port = htons(HANGMAN_TCP_PORT);							// Server port

 	if (bind(sock, (struct sockaddr *) & server, sizeof(server)) <0) {				// socket(), bind(), listen() -> Server prepared to accept connection
 		perror ("binding socket");
	 	exit (2);
 	}

 	listen (sock, 5);										// socket(), bind(), listen() -> server passive open

	drawHangman();											// Draw the hangman graphic

	printf("Server now running on port: %d \n", ntohs(server.sin_port));				// Display port number

 	while (1) {
 		client_len = sizeof (client);
 		if ((fd = accept (sock, (struct sockaddr *) &client, &client_len)) <0) {
 			perror ("accepting connection");
 			exit (3);
 		}

		// Display Client address and port
		if (inet_ntop(AF_INET, &client.sin_addr.s_addr, clntName,sizeof(clntName)) != NULL){
  			printf("Handling client %s/%d\n", clntName, ntohs(client.sin_port));
		}

 		play_hangman (fd, fd);									// Play the game
 		close (fd);										// Close the connection to client
 	}
 }

/* ---------------- Play_hangman () ---------------------*/

 void play_hangman (int in, int out)
 {
 	char * whole_word, part_word [LINESIZE],
 	guess[LINESIZE], outbuf [LINESIZE];
 	int i, word_length, lives = MAX_LIVES, game_state = 'I';					// Iterator, length of whole_word, correct guesses, game state
 	
  	displayHostname(out, outbuf);									// Display the name of the server

 	whole_word = whole_word = selectRandomWord(clntName, CLI_PORT);					// Select random word from words array
 	word_length = strlen(whole_word);								// Get length of word

 	/* No letters are guessed Initially */
 	for (i = 0; i <word_length; i++)
 		part_word[i]='-';									// Display hyphens to indicate size of word
 	
	part_word[i] = '\0';										// Add string terminating character

 	sprintf (outbuf, "%s %d \n", part_word, lives);							// Set outbuf to the part word & the lives left
 	write (out, outbuf, strlen(outbuf));								// Send to client

 	while (game_state == 'I')									// Loop until game is complete
 	/* Get a letter from player guess */
 	{
		while (read (in, guess, LINESIZE) <0) {							// Read guess from client
 			if (errno != EINTR)								// Check for error
 				exit (4);
 			printf ("re-read the startin \n");
 		} 											/* Re-start read () if interrupted by signal */

    		if (!correctGuess(whole_word, part_word, guess)) lives--;				// If it's an incorrect guess, decrement the number of lives

 		if (strcmp (whole_word, part_word) == 0)						// If all letters guessed correctly
 			game_state = 'W'; 								/* W ==> User Won */
 		else if (lives == 0) {									// If client has run out of guesses
 			game_state = 'L'; 								/* L ==> User Lost */
 			strcpy (part_word, whole_word); 						/* User Show the word */
 		}
 		sprintf (outbuf, "%s %d \n", part_word, lives);						// Set the part_word and lives to be sent
 		write (out, outbuf, strlen (outbuf));							// Send outbuf info to client
	
    		checkGameOver(game_state, outbuf, whole_word, out, clntName, CLI_PORT); 		// If game is finished, display win/lose message
 	}
 }
