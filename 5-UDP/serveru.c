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
	20171116 	CreateUDPSocket.h - abstracts the UDP socket creation code
	20171115 	Fixed client continuing to connect, 
			add command line parameter check
			Fixed warning messages by casting sockaddr_in to struct sockaddr*
			And moved HandleErrors to parent folder	
			Added random word selection	
*/

#include <unistd.h>											// write(), gethostname()
#include <string.h>											// strlen()
#include <stdlib.h>											// exit(), rand(), srand()
#include <time.h>											// time(), Seed the random number
#include "../HandleErrors.h"										// Moved HandleErrors.h to parent folder
#include "../DrawHangman.h"										// Draw graphics and add colour, needs to be included before Hangman.h
#include "../Hangman.h"
#include "../CreateUDPSocket.h"
#include "../UDPPlayHangman.h"


char username[LINESIZE];										// The username entered by the Client

void play_hangman (int in, int out);

//struct sockaddr_in srvAddr;
struct sockaddr_in cliAddr;										// IPv4 address structure

int slen = sizeof(cliAddr);
#define CLI_PORT ntohs(cliAddr.sin_port)								// Format the port number

int main(void) {
	int sock, i, count;
	char buf[LINESIZE];

	sock = createUDPServer();									// Create the UDP Server socket

	drawHangmanNew();										// DrawHangman.h: Draw the game logo
	//printf("\nWaiting For Data...\n");

	while(1) {
		fflush(stdout);
		srand(time(NULL));
// RECEIVE USERNAME
		if((count = recvfrom(sock,username,LINESIZE,0,(struct sockaddr*) &cliAddr, &slen))==-1) {// Server receives 1st, recvfrom returns -1 if error
			//displayErrMsg("Username recvfrom() Failed");
			break;										// Just leave the loop without fuss
			printf("No more input from %s%s%s\n",BLUE,username,NORM);			// Display end of input message
		}
		
		username[count-1] = '\0';

		printf("Username received: %s%s%s\n",BLUE,username,NORM);				// Format and display the username

		play_hangman(sock,sock);
		printf("Finished Playing Hangman for %s%s%s\n",BLUE,username,NORM);	
		
		//shutdown(sock, SHUT_WR);
		close(sock);	
	}

	//close(sock);	

	return 0;
}

/* ---------------- Play_hangman () UDP ---------------------*/
void play_hangman (int in, int out) {
 	char * whole_word, part_word [LINESIZE],guess[LINESIZE], outbuf[LINESIZE];
 	int lives = MAX_LIVES, game_state = 'I', i, good_guess, word_length;
 	char hostname[LINESIZE];

 	gethostname (hostname, LINESIZE);
 	sprintf(outbuf, "Playing hangman on host %s: \n \n", hostname);			
 	write(0, outbuf, strlen (outbuf));						
 	
	whole_word = selectRandomWord(username, CLI_PORT);						// Hangman.h: Username and Port instead of IP and Port
 	word_length = strlen(whole_word);
 	
	initPartWord(part_word, word_length);								// Hangman.h: Set the part word to hyphens
/*
 	for (i = 0; i < word_length; i++) {
		part_word[i] = '-';
	} 
	part_word[i] = '\0';
*/
 	sprintf (outbuf, "%s %d \n", part_word, lives);	
	sendto(out, outbuf, strlen(outbuf), 0, (struct sockaddr*) &cliAddr, sizeof cliAddr);	

	while (game_state == 'I') {						
		if (recvfrom(in, guess, LINESIZE, 0, (struct sockaddr *) &cliAddr, &slen) == -1){	// cast sockaddr_in to sockaddr
			 displayErrMsg("Guess recvfrom() Failed");
		}

		//good_guess = correctGuess(whole_word, part_word, guess);				// Hangman.h: Check if a correct guess has been made
/*
 		good_guess = 0;

 		for (i = 0; i < word_length; i++) {			
 			if (guess [0] == whole_word[i]) {				
 				good_guess = 1;						
 				part_word [i] = whole_word[i];			
 			}
 		}
*/
		//lives = checkGuessUDP(outbuf, whole_word, part_word, guess, lives, name, port);
		lives = checkGuessUDP(outbuf, whole_word, part_word, guess, lives);			// UDPPlayHangman.h: check the client guess, and display formatted message on server side



 		//if (!good_guess) lives--;	

		game_state = checkGameState(whole_word, part_word, lives);				// Hangman.h: If all letters guessed, W, if out of guesses/lives, L, otherwise I is returned
/*
 		if (strcmp (whole_word, part_word) == 0) game_state = 'W';

 		else if (lives == 0)  {
 			game_state = 'L'; 						
 			strcpy (part_word, whole_word); 
 		}
*/

 		sprintf (outbuf, "%s %d \n", part_word, lives);						// Format the part word and lives as a string to send to the client
		sendto(out, outbuf, LINESIZE, 0, (struct sockaddr *) &cliAddr, slen);			// cast sockaddr_in to sockaddr

		printf("Game State: %c Lives: %d\n", game_state, lives);				// Display the game state and lives, for testing
	}	

/* GAME TERMINATES WITH 'b' FROM 'bye' */
	sprintf (outbuf, "%s\n", "bye");								// The Client tests for the letter 'b' arriving as the first letter, as a condition to exit it's loop
	sendto(out, outbuf, LINESIZE, 0, (struct sockaddr *) &cliAddr, slen);				// cast sockaddr_in to sockaddr

	close(in);											// Close the socket connection
}
