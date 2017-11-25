 /* 
 	File: 		clientu.c
	Version: 	Hangman UDP Client
	Modified by: 	Samantha Marah
			Joe O'Regan

	Year 4 Networked Games Assignment

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690

	20171122	Added functionality in Hangman.h to make code more modular			
	20171116 	CreateUDPSocket.h - abstracts the UDP socket creation code
	20171115 	Fixed client continuing to connect, 
			add command line parameter check
*/


#include <stdio.h>							// printf()
#include <string.h>							// strlen()
#include <stdlib.h>							// exit(), rand()
#include "../DrawHangman.h"						// Display graphics and add colour
#include "../HandleErrors.h"						// Display error messages, and handle program exit
#include "../Hangman.h"							// Functions for playing hangman
#include "../CreateUDPSocket.h"						// Functions to create UDP sockets

int DRAW_HANGMAN_GFX = 0;						// Set whether to draw the graphics, 0 don't draw, 1 do draw

int main (int argc, char * argv []) {					// Added arguments to specify another IP address other than the default localhost/127.0.0.1
	struct sockaddr_in si_other;
 	int s, byteCount, win;						// The socket, number of bytes recieved, and decide which end of game message to display
	int slen=sizeof(si_other);					// Size of the socket address
	char userInput[LINESIZE];					// Send buffer, read in guess and sent to Server
	char partword[LINESIZE];					// Input buffer, read in part word from Server

	s = createUDPClient((argc == 1) ? SRV_IP : argv[1]);		// Create the IPv4 UDP socket
	si_other = getServerAddress((argc == 1) ? SRV_IP : argv[1]);	// Set up the address stucture for the server
// Enter username, to be used to check against stored states
	printf("Please Enter Your Username: ");				// Get username
	fgets(userInput, LINESIZE, stdin);				// Enter username		

/*SEND*/sendGuess(s, userInput, si_other);				// CreateUDPSocket.h: Send the username input on the client side to the server

// PLAY HANGMAN GAME
	while(1) {							// change to check lives, as this will exit on word beginning with b
		partword[0]='\0';					// Reset the part-word string

/*RECV*/	if (byteCount = recvfrom(s, &partword, LINESIZE, 0, (struct sockaddr *) &si_other, &slen) == -1) break;	
		//printf("partword: %s\n", partword);			// Test part word received OK
		if (partword[0] == 'b') break;				// Server sends "bye" when the game is finished, a special character would be a better idea
		win = parseWordAndLives(partword, DRAW_HANGMAN_GFX);	// Hangman.h: parse the word and display it on screen, and set the game over message in win
		
		fgets(userInput, LINESIZE, stdin);			// Get the Players guess from the keyboard, standard input
/*SEND*/	sendGuess(s, userInput, si_other);			// CreateUDPSocket.h: Send the guess input on the client side to the server
	}

	gameOverText(win);						// DrawHangman.h: Display the game over message, depending on how many lives/guesses the player has left

	close(s);							// Close the socket connection
	exit(0);							// Exit the program
	
	return 0;
}
