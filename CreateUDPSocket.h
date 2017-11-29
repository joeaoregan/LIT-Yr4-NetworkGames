 /* 
 	File: 		CreateUDPSocket.h
	Version: 	Functions to create and connect UDP sockets
	Author:		Joe O'Regan

	Year 4 Networked Games Assignment

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690

	16/11/2017

	Reconfigured version of SetupTCPServerSocket() function from TCPServerUtility.c
	from TCP/IP Sockets in C book, to work with UDP Hangman server
	Using https://gafferongames.com/post/sending_and_receiving_packets/ as a reference
	
	25/11/2017	Added option to specify port for server to run on
*/

#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include "Hangman.h"
#include "AddressFunctions.h"
#include "HandleErrors.h"							// Display Error messages
#include "Socket.h"

#define UDP_PORT "1068"								// The port number the Hangman Server will run on
#define UDP_PORT_NUM 1068							// The port number the Hangman Server will run on
#define SRV_IP "127.0.0.1"							// IPv4 Address of server on local machine

/*
	SERVER:
	Function to create and bind UDP Server socket
*/
int createUDPServer(char* port) {
	struct sockaddr_in srvAddr;
	int server;

	if ((server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) 
		displayErrMsg("Socket CreationFailed");				// Create a UDP socket, if this fails display an error message
	else printf("Hangman UDP Socket Created ");

	memset((char *) &srvAddr, 0, sizeof(srvAddr));				// Set/Reset the IP address to 0

	srvAddr.sin_family = AF_INET;						// Specify the address family as IPv4
	srvAddr.sin_port = htons(atoi(port));						// Specify the port number entered by user, or default 1068
	srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);				// Use IPv4 address, could be more than one

	if (bind(server, (struct sockaddr *) &srvAddr, sizeof(srvAddr)) == -1) 	// Bind to the address stored in srvAddr
		displayErrMsg("but Bind Failed");				// HandleErrors.h: display error if -1 returned by call to bind()
	printf("and Bind Successful\n");

	return server;								// Return the server socket
}


/*
	CLIENT:
	Function to create and return UDP Client socket
*/
int createUDPClient(char* server_name){
	struct sockaddr_in srvAddr;						// IPv4 address stucture
	int client;

	if ((client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)  	// create the socket
		displayErrMsg("Socket Failed");					// HandleErrors.h: display error if -1 returned by call to socket()
	else printf("UDP Hangman Socket Created\n");
		
	return client;								// Return the client socket
}


/*
	SERVER:
	Get the address of the server using inet_aton(), and return an IPv4 sockaddr_in address structure.
*/
struct sockaddr_in getServerAddress(char* server_name, char* port){
	struct sockaddr_in srvAddr;						// IPv4 address stucture

	memset((char *) &srvAddr, 0, sizeof(srvAddr));				// Zero out the address, could also use bzero()

	srvAddr.sin_family = AF_INET;						// Set the address family as IPv4
	srvAddr.sin_port = htons(atoi(port));					// Set the port to the default stored port

	if (inet_aton(server_name, &srvAddr.sin_addr) == 0 )  			// inet_aton() replaced by inet_pton()
		displayErrMsgStatus("Sinet_aton() failed", 1);			// HandleErrors.h: Display an error message with exit status of 1

	return srvAddr;								// Return the IPv4 address structure
}

