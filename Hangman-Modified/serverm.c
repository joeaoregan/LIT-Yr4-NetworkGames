 /* 
 	File: 		serverm.c
	Version: 	Modified version of Hangman Server
	Author:		Joe O'Regan

	Year 4 Networked Games Assignment

	Network server for hangman game

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690

	18/11/2017	Moved play_hangman functionality to TCPPlayHangman.h
	23/09/2017 	Added includes to get rid of compile warnings
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
#include "../CreateTCPSocket.h"
#include "../HandleErrors.h"
#include "../TCPPlayHangman.h"										// 18/11/2017 Moved hang_man() function to separate file

extern time_t time ();			

char clntName[INET_ADDRSTRLEN];										// Client address string
struct sockaddr_in server, client;									// Made global to display client name outside main
#define CLI_PORT ntohs(client.sin_port)

void main ()												// No command line arguments
{
 	int sock, fd, client_len;							

 	srand ((int) time ((long *) 0)); 								// randomize the seed

	sock = createTCPServerSocket();

	drawHangman();											// Draw the hangman graphic

	printf("Server now running on port: %d \n", ntohs(server.sin_port));				// Display port number

 	while (1) {
 		client_len = sizeof (client);
 		if ((fd = accept (sock, (struct sockaddr *) &client, &client_len)) < 0) 		// Create the listening socket, and if its return value is less than 0
			displayErrMsgStatus("Accepting Connection", 3);					// Display error message, and exit with return status 3

		// Display Client address and port
		if (inet_ntop(AF_INET, &client.sin_addr.s_addr, clntName,sizeof(clntName)) != NULL){	// Convert the address to a string, and store in clntName
  			printf("Handling client %s/%d\n", clntName, ntohs(client.sin_port));		// Display the client IP address and port number
		}

 		play_hangman (fd, fd);									// Play the game
 		close (fd);										// Close the connection to client
 	}
 }

/* ---------------- Play_hangman () ---------------------*/

 void play_hangman (int in, int out)
 {
 	char * whole_word, part_word [LINESIZE],							// LINESIZE is declared in Hangman.c replacing MAXLEN
 	guess[LINESIZE], outbuf [LINESIZE];
 	int i, word_length, lives = MAX_LIVES, game_state = 'I';					// Iterator, length of whole_word, correct guesses, game state
 	
  	displayHostname(out, outbuf);									// Display the name of the server

 	whole_word = whole_word = selectRandomWord(clntName, CLI_PORT);					// Select random word from words array
 	word_length = strlen(whole_word);								// Get length of word

 	/* No letters are guessed Initially */
 	for (i = 0; i < word_length; i++)								// For every character in the word
 		part_word[i]='-';									// Display hyphens to indicate size of word
 	
	part_word[i] = '\0';										// Add string terminating character

 	sprintf (outbuf, "%s %d \n", part_word, lives);							// Set outbuf to the part word & the lives left
 	write (out, outbuf, strlen(outbuf));								// Send to client

 	while (game_state == 'I') {									// Loop until game is complete
		// Get a letter from player guess
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
