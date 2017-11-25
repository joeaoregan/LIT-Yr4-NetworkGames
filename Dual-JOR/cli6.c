// Jason Foley, Joe O'Regan
// Hangman client connecting with IPv4 and IPv6 addresses
/* Hangclient.c - Client for hangman server.  */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>		// IPv6
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include "../HandleErrors.h"	// XXX error handling functions header in parent directory
#include "../DrawHangman.h"
#include "../CreateTCPSocket.h"

#define LINESIZE 80

int main (int argc, char * argv [])
{
//	struct sockaddr_in6 server; 	/* Server's address assembled here */ 	// XXX sockaddr_in6 holds IPv6 address
	struct sockaddr_storage server; /* Server's address assembled here */ 	// XXX sockaddr_in6 holds IPv6 address
	struct hostent * host_info;						// XXX
	int sock, count;
	char i_line[LINESIZE];
	char o_line[LINESIZE];
	char * server_name;

 	/* Get server name from the command line.  If none, use 'localhost' */
 //	server_name = (argc = 1)?  argv [1]: "localhost";			// XXX this is broken
//	server_name = (argc == 1) ? "localhost" : argv[1];



	int sockType = createTCPClientDualStack(argv[1], TCP_PORT_NUM);
	printf("Socket Type IPv%d\n",sockType);

	if (sockType == 4) {
		struct sockaddr_in server;	
//		server = createTCPClientSocket(&sock, 				// CreateTCPSocket.h: Create the connection between the client & server, 
		IPv4TCPClientSocket(&sock, 					// CreateTCPSocket.h: Create the connection between the client & server, 
			(argc == 1) ? SRV_IP : argv[1], 			// use default IP address of localhost if none entered
			(argc == 3) ? atoi(argv[2]) : TCP_PORT_NUM);		// If 3 parameters are entered, use the given port number, otherwise, use default 1066
	} else if (sockType == 6) {
		struct sockaddr_in6 server;
		IPv6TCPClientSocket(&sock, 					// CreateTCPSocket.h: Create the connection between the client & server, 
			(argc == 1) ? SRV_IP : argv[1], 			// use default IP address of localhost if none entered
			(argc == 3) ? atoi(argv[2]) : TCP_PORT_NUM);		// If 3 parameters are entered, use the given port number, otherwise, use default 1066
	}

/*

 	// Create the socket
 	sock = socket(AF_INET6, SOCK_STREAM, 0);				// XXX
 	if (sock < 0) displayErrMsgStatus("Creating stream socket", 1);		// XXX error handling function in HandleErrors.h

 	host_info = gethostbyname2(server_name,AF_INET6);
 	if (host_info == NULL) {
 		fprintf (stderr, "%s: unknown host: %s \n", argv [0], server_name);
 		exit (2);
 	}

 	// Set up the server's socket address, then connect

 	server.sin6_family = AF_INET6;
 	//inet_pton ((char *) & server.sin6_addr, host_info->h_addr, host_info->h_length);
  	inet_pton(AF_INET6, "::1",(char *) & server.sin6_addr);
 	server.sin6_port = htons (HANGMAN_TCP_PORT);

 	if (connect (sock, (struct sockaddr *) & server, sizeof server)) 
		displayErrMsgStatus("Connecting to server", 3); 		// XXX error handling function in HandleErrors.h

 	printf ("Connected to server %s \n", server_name);


*/

 	//OK connected to the server.  
 	//Take a line from the server and show it, take a line and send the user input to the server. 
 	//Repeat until the server terminates the connection.

 	while ((count = read (sock, i_line, LINESIZE)) > 0) {
 		write (1, i_line, count);
 		count = read (0, o_line, LINESIZE);//0 = STDIN
 		write (sock, o_line, count);
 	}

	return 0;

 }
