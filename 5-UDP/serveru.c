 /* 
 	File: 		serveru.c
	Version: 	Hangman UDP Server
	Modified by:	Samantha Marah
			Joe O'Regan

	Year 4 Networked Games Assignment

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690

	20171122	Added in Hangman.h functions, to make code more modular 
			(and easier to fit in screen shots)
			Created GetUserNameUDP() function to read in the Player name
	20171116 	CreateUDPSocket.h - abstracts the UDP socket creation code
	20171115 	Fixed client continuing to connect, 
			add command line parameter check
			Fixed warning messages by casting sockaddr_in to struct sockaddr*
			And moved HandleErrors to parent folder	
			Added random word selection	
*/

#include <string.h>											// strlen()
#include <stdlib.h>											// exit(), rand(), srand()
#include <time.h>											// time(), Seed the random number
#include "../HandleErrors.h"										// Moved HandleErrors.h to parent folder
#include "../DrawHangman.h"										// Draw graphics and add colour, needs to be included before Hangman.h
#include "../Hangman.h"											// Functions to play hangman
#include "../CreateUDPSocket.h"										// Functions to create and use UDP sockets
#include "../UDPPlayHangman.h"										// Functions to play hangman using sendto() and recvfrom()
#include "../Socket.h"											// Socket functions intended for use with both TCP and UDP; displayHostname()

void playHangmanUDP(int in, int out, char* name);

struct sockaddr_in cliAddr;										// IPv4 address structure

int slen = sizeof(cliAddr);
#define CLI_PORT ntohs(cliAddr.sin_port)								// Format the port number

int main(void) {
	int sock, count;										// Client socket, and byte count variables
	char buf[LINESIZE], username[LINESIZE];								// Buffer string, the username entered by the Client

	sock = createUDPServer();									// Create the UDP Server socket

	drawHangmanNew();										// DrawHangman.h: Draw the game logo

	while(1) {
		fflush(stdout);										// flushes the output buffer of a stream
		srand(time(NULL));									// Randomise the seed, to get a different word each game
/* RECEIVE USERNAME
		if((count = recvfrom(sock,username,LINESIZE,0,(struct sockaddr*) &cliAddr, &slen))==-1) {// Server receives 1st, recvfrom returns -1 if error
			//displayErrMsg("Username recvfrom() Failed");
			printf("No more input from %s%s%s\n",BLUE,username,NORM);			// Display end of input message
			break;										// Just leave the loop without fuss or errors, if no more input is received
		} else {		
			username[count-1] = '\0';							// Terminate the end of the string (before the '\n' new line character)
			printf("Username received: %s%s%s\n",BLUE,username,NORM);			// Format and display the username
		}
*/
		//if(getUserNameUDP(sock,username,cliAddr)){						// If a user name is input
		getUserNameUDP(sock,username,cliAddr);							// If a user name is input
		//	printf("input: %s\n",username);
			playHangmanUDP(sock,sock,username);						// Play the game
		//}

//	printf("input2: %s\n",username);
		printf("Finished Playing Hangman for %s%s%s\n",BLUE,username,NORM);			// Inform on server side the game has finished
		
		//shutdown(sock, SHUT_WR);
		close(sock);										// Close the socket connection
	}	

	return 0;											// End program
}

/* ---------------- Play_hangman () UDP ---------------------*/
void playHangmanUDP(int in, int out, char* name) {
 	char * whole_word, part_word [LINESIZE],guess[LINESIZE], outbuf[LINESIZE];		// Selected word, part word, input buffer, output buffer
 	int lives = MAX_LIVES, game_state = 'I', i, word_length;
 /*	char hostname[LINESIZE];

 	gethostname(hostname, LINESIZE);
 	sprintf(outbuf, "Playing hangman on host %s: \n \n", hostname);			
 	write(0, outbuf, strlen (outbuf));						
 */

	printf("name test 3: %s\n", name);

	displayHostname();									// Socket.h: Display the local machines name

	printf("test hostname\n");
	
	whole_word = selectRandomWord(name, CLI_PORT);						// Hangman.h: Username and Port instead of IP and Port
 	word_length = strlen(whole_word);
 	
	printf("test before init part word\n");
	initPartWord(part_word, word_length);							// Hangman.h: Set the part word to hyphens

// 	sprintf (outbuf, "%s %d \n", part_word, lives);	
//	sendto(out, outbuf, strlen(outbuf), 0, (struct sockaddr*) &cliAddr, sizeof cliAddr);	

	printf("test before while\n");
	while (game_state == 'I') {		
/*SEND*/
	printf("test inside while\n");
 		sprintf (outbuf, "%s %d \n", part_word, lives);					// Format the part word and lives as a string to send to the client
		sendto(out, outbuf, LINESIZE, 0, (struct sockaddr *) &cliAddr, slen);		// cast sockaddr_in to sockaddr
	printf("outbuf: %s\n", outbuf);
	printf("test after send\n");
				
/*RECV*/	if (recvfrom(in,guess,LINESIZE,0, (struct sockaddr *) &cliAddr, &slen) == -1) {	// cast sockaddr_in to sockaddr
			 //displayErrMsg("Guess recvfrom() Failed");
			printf("Hangman recvfrom: No more input from %s%s%s\n",BLUE,name,NORM);	// Display end of input message
			break;
		}

	printf("test after receive\n");
		//lives = checkGuessUDP(outbuf, whole_word, part_word, guess, lives, name, port);
		lives = checkGuessUDP(outbuf, whole_word, part_word, guess, lives);		// UDPPlayHangman.h: check the client guess, and display formatted message on server side

		game_state = checkGameState(whole_word, part_word, lives);			// Hangman.h: If all letters guessed, W, if out of guesses/lives, L, otherwise I is returned


		printf("Game State: %c Lives: %d\n", game_state, lives);			// Display the game state and lives, for testing
	}	

/* GAME TERMINATES WITH 'b' FROM 'bye' */
	sprintf (outbuf, "%s\n", "bye");							// The Client tests for the letter 'b' arriving as the first letter, as a condition to exit it's loop
/**/	sendto(out, outbuf, LINESIZE, 0, (struct sockaddr *) &cliAddr, slen);			// cast sockaddr_in to sockaddr

	close(in);										// Close the socket connection
}
