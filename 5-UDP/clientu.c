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

	20171116 	CreateUDPSocket.h - abstracts the UDP socket creation code
	20171115 	Fixed client continuing to connect, 
			add command line parameter check
*/

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "../DrawHangman.h"						// Display graphics and add colour
#include "../HandleErrors.h"
#include "../Hangman.h"
#include "../CreateUDPSocket.h"

int DRAW_HANGMAN_GFX = 0;						// Set whether to draw the graphics, 0 don't draw, 1 do draw

int main (int argc, char * argv []) {					// Added arguments to specify another IP address other than the default localhost/127.0.0.1
	struct sockaddr_in si_other;
 	int s, byteCount, win;						// The socket, number of bytes recieved, and decide which end of game message to display
	int slen=sizeof(si_other);
	char userInput[LINESIZE];
	char partword[LINESIZE];

	s = createUDPClient((argc == 1) ? SRV_IP : argv[1]);		// Create the IPv4 UDP socket
	si_other = getServerAddress((argc == 1) ? SRV_IP : argv[1]);	// Set up the address stucture for the server
// Enter username
	printf("Please Enter Your Username: ");				// Get username
	fgets(userInput, LINESIZE, stdin);				// Enter username		

	sendGuess(s, userInput, si_other);				// CreateUDPSocket.h: Send the username input on the client side to the server

// PLAY THE GAME
	while(1) {							// change to check lives, as this will exit on word beginning with b
	//while(partword[0] != 'b') {					// Works, but lives affected - change to check lives, as this will exit on word beginning with b
	//while((byteCount = recvfrom(s, &partword, LINESIZE, 0, (struct sockaddr *) &si_other, &slen)) > -1) {	// NOT WORKING - change to check lives, as this will exit on word beginning with b
// RECEIVE THE PART WORD
		//partword[0]='\0';					// Reset the part-word string

		if(byteCount = recvfrom(s, &partword, LINESIZE, 0, (struct sockaddr *) &si_other, &slen) == -1) break;	 
		if (partword[0] == 'b') break;
		//if (byteCount == 0) break;	
		//partword[byteCount-1] = '\0'; 			// bytecount -1 to remove \n or \r or whatever is giving new line		
		//printf("Received:  %s", partword);			// Display the string received from the server	
		win = parseWordAndLives(partword, DRAW_HANGMAN_GFX);	// Hangman.h: parse the word and display it on screen, and set the game over message in win

		fgets(userInput, LINESIZE, stdin);

		sendGuess(s, userInput, si_other);			// CreateUDPSocket.h: Send the guess input on the client side to the server
	}

	//printf("Client Finished\n");
	gameOverText(win);						// Display the game over message, depending on how many lives/guesses the player has left

	close(s);
	exit(0);

	return 0;
}


