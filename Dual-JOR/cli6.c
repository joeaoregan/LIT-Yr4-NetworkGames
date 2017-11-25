// Jason Foley, Joe O'Regan
// Hangman client connecting with IPv4 and IPv6 addresses
/* Hangclient.c - Client for hangman server.  */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>								// IPv6
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include "../HandleErrors.h"							// XXX error handling functions header in parent directory
#include "../DrawHangman.h"
#include "../CreateTCPSocket.h"

#define LINESIZE 80

int main (int argc, char * argv []) {
	int sock, count;							// Client socket, and byte count for data
	char i_line[LINESIZE];							// Input buffer
	char o_line[LINESIZE];							// Output buffer

	/* IF THE USER ENTERS AN IP PARAMETER, DECIDE IF IPv4 (DEFAULT) OR IPv6 */
	int sockType = createTCPClientDualStack(argv[1], TCP_PORT_NUM);
	printf("Socket Type IPv%d\n",sockType);
	
/*
	if (sockType == 4) {
		IPv4TCPClientSocket(&sock, 					// CreateTCPSocket.h: Create the connection between the client & server, 
			(argc == 1) ? SRV_IP : argv[1], 			// use default IP address of localhost if none entered
			(argc == 3) ? atoi(argv[2]) : TCP_PORT_NUM);		// If 3 parameters are entered, use the given port number, otherwise, use default 1066
	} else if (sockType == 6) {
		IPv6TCPClientSocket(&sock, 					// CreateTCPSocket.h: Create the connection between the client & server, 
			(argc == 1) ? SRV_IP : argv[1], 			// use default IP address of localhost if none entered
			(argc == 3) ? atoi(argv[2]) : TCP_PORT_NUM);		// If 3 parameters are entered, use the given port number, otherwise, use default 1066
	}
*/
	// Previous 2 functions combined into 1

	/*
	   CREATE A SOCKET:
	   If the user entered parameters, use them, otherwise use default values
	   of IPv4 localhost 127.0.0.1 on port 1066, sockType decides the protocol
	   and defaults to IPv4 if no command line parameters were read.
	*/
	createClientSocketType(&sock, 						// CreateTCPSocket.h: Create the connection between the client & server, 
			(argc == 1) ? SRV_IP : argv[1], 			// use default IP address of localhost if none entered
			(argc == 3) ? atoi(argv[2]) : TCP_PORT_NUM,		// If 3 parameters are entered, use the given port number, otherwise, use default 1066
			sockType);						// sockType variable is used to decide to create IPv4 or IPv6 socket

 	/*
	   CONNECTED TO THE SERVER: 
 	   Take a line from the server and show it, 
	   take a line and send the user input to the server. 
 	   Repeat until the server terminates the connection.
	*/

 	while ((count = read(sock, i_line, LINESIZE)) > 0) {			// Read input from the server until the server closes the connection so read returns 0
 		write(1, i_line, count);					// Display the part word to the user using standard input
 		count = read(0, o_line, LINESIZE);				// Get the Player guess from standard input. 0 = STDIN
 		write(sock, o_line, count);					// Send the guess to the Server to be processed
 	}

	return 0;								// End the program
 }
