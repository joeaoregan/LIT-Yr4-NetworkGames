/* 
 	File: 		cliSendRecv.c
	Version: 	Modified Hangman Client using send() and recv()
	Modified by:	Joe O'Regan

	Year 4 Networked Games Assignment

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690

	22/11/2017 	Removed additional read after while loop, can't remember why it was there
			gameOverText() in DrawHangman.h sets a game over message based on the number
	 		of lives the player has left
			parseWordAndLives() moved to Hangman.h from DrawHangman.h, function now takes
			a draw paramater and calls amputate if set, or can be left as text only
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
#include "../DrawHangman.h"							// Functions to Draw Hangman Graphics
#include "../Hangman.h"								// Hangamen header file
#include "../CreateTCPSocket.h"							// Functions to create and connect sockets moved here

#define DRAW 1									// 0 = Don't Draw Hangman Graphics, 1 = Draw Hangman Graphics

 int main (int argc, char * argv []) {
 	struct sockaddr_in server; 						// Server's address assembled here
 	int sock, count, livesLeft;						// The socket, size of buffer data in bytes, game over message displayed based on lives left
 	char inbuf[LINESIZE];							// input buffer (linesize definition (80) moved to Hangman.h headf file)
 	char outbuf[LINESIZE];							// output buffer

	/* ----------------------CREATE SOCKET CONNECTION-----------------------*/
	if (argc < 1 || argc > 3)
		displayErrMsg("Parameters: [<IP Address>] [<Port>]");		// Parameters include optional IP address and port number, defaults are set for both IP & port
			
	server = createTCPClientSocket(&sock, 					// CreateTCPSocket.h: Create the connection between the client & server, 
			(argc == 1) ? SRV_IP : argv[1], 			// use default IP address of localhost if none entered
			(argc == 3) ? atoi(argv[2]) : TCP_PORT_NUM);		// If 3 parameters are entered, use the given port number, otherwise, use default 1066

	drawHangmanLogo(TCP_CLI_SR);						// DrawHangman.h: Draw Hangman Graphic Representation of Number of Lives Left

	/* ----------------------CONNECTED TO THE SERVER--------------------------
	* Take a line containing the part word from the server and display it
	* Get the guess from the user as keyboard input, and send to the server. 
	* Where the server processes the guess, determining if is correct, and 
	* also handles the win conditions for the game.
 	* Repeat until the server terminates the connection.
	------------------------------------------------------------------------*/

 	while ((count = recv(sock, inbuf, LINESIZE, 0)) > 0) {			// While there is input from the Server (Part word received from the Server)
										// return -1 error, 0 socket closed remotely
		livesLeft = parseWordAndLives(inbuf, DRAW);			// DrawHangman.h: Parse input from Server & Display lives left & partword, with optional graphics
 	    	count = read(0, outbuf, 3);					// 0 = Standard Input. Get input from Client (Enter guess letter)
	    	if ((send(sock, outbuf, count, 0)) < 0)				// Send Client input to Server
			displayErrMsg("Recv failed");
 	}

	gameOverText(livesLeft);						// DrawHangman.h: Select game over message to display, with 3 different messages, win/lose/perfect

	close(sock);								// Close the socket
	exit(0);								// Exit the application
}

