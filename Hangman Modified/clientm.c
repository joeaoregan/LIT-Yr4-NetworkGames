 /* 
	Client for hangman server

 	File: Hangclient.c

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690

	12/10/2017	Added header files for shared game variables and methods
	19/09/2017 	Added includes to get rid of compile warnings (wasn't necessary)
			Game Over message displayed after connection to Server terminates
	23/09/2017	Added include to clear warning on read(), write()
			Added hangman graphic
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>		// 19/09/2017 Warning for memcpy
#include <stdlib.h>		// 19/09/2017 Warning for exit
#include <unistd.h>		// 24/09/2017 gethostname(), write(), read(), close()
#include "../Hangman.h"		// 11/10/2017 Hangamen header file
#include "../DrawHangman.h"	// 11/10/2017 Draw Hangman Graphic
#include "../CreateTCPSocket.h"

 int main (int argc, char * argv []) {
 	struct sockaddr_in server; /* Server's address assembled here */
 	struct hostent * host_info;
 	int sock, count;									// The socket, and the size of the data to receive
 	char i_line[LINESIZE];									// input buffer (linesize definition (80) moved to Hangman.h headf file)
 	char o_line[LINESIZE];									// output buffer
 //	char * server_name;									// The name for the server, input as a command line argument
 	
//	server_name = (argc == 1) ?  SRV_IP : argv[1];						// Get server name from the command line.  If none, use 'localhost' - fixed ternary operator was "=" instead of "=="

/*
 // CREATE THE SOCKET
 	sock = socket (AF_INET, SOCK_STREAM, 0);
 	if (sock < 0) displayErrMsgStatus("Creating Stream Socket", 1);	

 	host_info = gethostbyname(server_name);
 	if (host_info == NULL) {
 	  fprintf (stderr, "%s: unknown host:%s \n", argv [0], server_name);
 	  exit (2);
 	}

// SET UP THE SERVER'S SOCKET ADDRESS, AND CONNECT
 	server.sin_family = host_info->h_addrtype;
 	memcpy ((char *) & server.sin_addr, host_info->h_addr, host_info->h_length);
 	server.sin_port = htons(TCP_PORT_NUM);

 	if (connect (sock, (struct sockaddr *) & server, sizeof server) < 0)			// If the connection fails
		displayErrMsgStatus("Connecting To Server", 3);					// Display the error message, and exit with status 3

 	printf ("Connected to server: %s \n", server_name);					// 19/09/2017 Change formatting		
*/
	server = createTCPClientSocket(&sock, (argc == 1) ?  SRV_IP : argv[1]);
	drawHangman();										// Draw Hangman Graphic Representation of Number of Lives Left

	/* -------------------------------CONNECTED TO THE SERVER---------------------------------
 	Take a line from the server and show it, take a line and send the user input to the server. 
 	Repeat until the server terminates the connection.
	----------------------------------------------------------------------------------------*/

/* PARSE VARIABLES */
	char arg1PartWord[20];
	int arg2LivesLeft;
/* SERVER SENDS ITS NAME */
	count = read (sock, i_line, 34);							// Read from server
	write (1, i_line, count);								// Display on screen

 	while ((count = read (sock, i_line, LINESIZE)) > 0) {
/* PARSE INPUT FROM SERVER */
		sscanf(i_line, "%s %d", &(*arg1PartWord), &arg2LivesLeft);			// Parse string data received from server into separate part-word and score variables
		amputate(arg2LivesLeft);							// Display graphical represenation of lives left
		printf("%s\n", arg1PartWord);
//	    	write (1, i_line, count);							// Display the data string received from the server on screen
 	    	count = read (0, o_line, LINESIZE);						// 0 = STDIN
 	    	write (sock, o_line, count);							// Send client input to server
 	}

	printf("Game Over!\n");									// Game over message to test prog terminates
}
