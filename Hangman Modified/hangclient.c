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

 int main (int argc, char * argv []) {
 	struct sockaddr_in server; /* Server's address assembled here */
 	struct hostent * host_info;
 	int sock, count;									// The socket, and the size of the data to receive
 	char i_line[LINESIZE];									// input buffer (linesize definition (80) moved to Hangman.h headf file)
 	char o_line[LINESIZE];									// output buffer
 	char * server_name;									// The name for the server, input as a command line argument

 	/* Get server name from the command line.  If none, use 'localhost' */

 	server_name = (argc = 1)?  argv [1]: "localhost";

 	/* Create the socket */
 	sock = socket (AF_INET, SOCK_STREAM, 0);
 	if (sock <0) {
 	  perror ("Creating stream socket");
 	  exit (1);
 	}

 	host_info = gethostbyname(server_name);
 	if (host_info == NULL) {
 	  fprintf (stderr, "%s: unknown host:%s \n", argv [0], server_name);
 	  exit (2);
 	}

 	/* Set up the server's socket address, then connect */

 	server.sin_family = host_info->h_addrtype;
 	memcpy ((char *) & server.sin_addr, host_info->h_addr, host_info->h_length);
 	server.sin_port = htons (HANGMAN_TCP_PORT);

 	if (connect (sock, (struct sockaddr *) & server, sizeof server) <0) {
 	  perror ("connecting to server");
 	  exit (3);
 	}
/* DRAW HANGMAN GRAPHIC - Graphical representation of number of lives left */
/**/	drawHangman();										// Draw hangman Graphic

 	/*OK connected to the server.  
 	Take a line from the server and show it, take a line and send the user input to the server. 
 	Repeat until the server terminates the connection. */

 	printf ("Connected to server: %s \n", server_name);					// 19/09/2017 Change formatting

	//int i, numLetters = 0;

	/* -------------------------------------------------------------------------------------*/
/* PARSE VARIABLES */
/**/	char arg1PartWord[20];
/**/	int arg2LivesLeft;
/* SERVER SENDS ITS NAME */
/**/	count = read (sock, i_line, 34);							// Read from server
/**/	write (1, i_line, count);								// Display on screen

 	while ((count = read (sock, i_line, LINESIZE)) > 0) {
/* PARSE INPUT FROM SERVER */
/**/		sscanf(i_line, "%s %d", &(*arg1PartWord), &arg2LivesLeft);			// Parse string data received from server into separate part-word and score variables
 		//printf("test: %s\n", arg1PartWord);						// Test parsing the buffer string from server worked
		//printf("Try A Guess: %s Lives Left: %d\n", arg1PartWord, arg2LivesLeft);	
/**/		amputate(arg2LivesLeft);							// Display graphical represenation of lives left
		printf("%s\n", arg1PartWord);
//	    	write (1, i_line, count);							// Display the data string received from the server on screen
 	    	count = read (0, o_line, LINESIZE);						// 0 = STDIN
 	    	write (sock, o_line, count);							// Send client input to server
 	}

/**/	printf("Game Over!\n");									// 19/09/2017 Add simple game over message to test prog terminates
}
