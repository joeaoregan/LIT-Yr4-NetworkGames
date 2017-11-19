 /* 
 	File: 		clientm.c
	Version: 	Modified Hangman Client
	Author:		Joe O'Regan

	Year 4 Networked Games Assignment

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690

	16/10/2017	amputate() in DrawHangman.h displays graphical representation of lives lost
	13/10/2017	DrawHangman.h added to create ASCII graphics
	12/10/2017	Added header files for shared game variables and methods
	19/09/2017 	Added includes to get rid of compile warnings (wasn't necessary)
			Game Over message displayed after connection to Server terminates
	23/09/2017	Added include to clear warning on read(), write()
			Added hangman graphic
*/

#include <stdio.h>								// printf(), snprintf()
#include <string.h>								// strlen()
#include <stdlib.h>								// exit()
#include <unistd.h>								// write(), read(), close()
#include "../Hangman.h"								// 11/10/2017 Hangamen header file
#include "../DrawHangman.h"							// 11/10/2017 Functions to Draw Hangman Graphics
#include "../CreateTCPSocket.h"							// 16/11/2017 Functions to create and connect sockets moved here

 int main (int argc, char * argv []) {
 	struct sockaddr_in server; 						// Server's address assembled here
 	int sock, count;							// The socket, and the size of the data to receive
 	char inbuf[LINESIZE];							// input buffer (linesize definition (80) moved to Hangman.h headf file)
 	char outbuf[LINESIZE];							// output buffer

	/* ------------------------------CREATE SOCKET CONNECTION-----------------------*/

	server = createTCPClientSocket(&sock, (argc == 1) ? SRV_IP : argv[1]);	// Create the connection between the client and the server, use default IP if none entered

	drawHangman();								// Draw Hangman Graphic Representation of Number of Lives Left

	/* -------------------------------CONNECTED TO THE SERVER-----------------
	* Take a line containing the part word from the server and display it, 
	* Get the guess from the user as keyboard input, and send to the server. 
	* Where the server processes the guess, determining if is correct, and 
	* also handles the win conditions for the game.
 	* Repeat until the server terminates the connection.
	------------------------------------------------------------------------*/

	count = read (sock, inbuf, 34);						// Read randomly generated part word from Server
	write (1, inbuf, count);						// 1 = Standard Output. Display on screen

 	while ((count = read (sock, inbuf, LINESIZE)) > 0) {			// While there is input from the Server (Part word received from the Server)
		amputate(inbuf);						// Parse input from Server & Display text/graphic of lives left & partword
 	    	count = read(0, outbuf, LINESIZE);				// 0 = Standard Input. Get input from Client (Enter guess letter)
 	    	write(sock, outbuf, count);					// Send Client input to Server
 	}

	count = read (sock, inbuf, 34);						// Read randomly generated part word from Server
	write (1, inbuf, count);						// 1 = Standard Output. Display on screen


	printf("%sGAME OVER!%s\n", RED, NORM);					// Game over message to test prog terminates
}
