/*
	CreateUDPSocket.h

	16/11/2017

	Reconfigured version of SetupTCPServerSocket() function from TCPServerUtility.c
	from TCP/IP Sockets in C book, to work with UDP Hangman server
	Using https://gafferongames.com/post/sending_and_receiving_packets/ as a reference
*/

#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include "AddressFunctions.h"
#include "HandleErrors.h"											// Display Error messages

#define HANGMAN_UDP_PORT 1068											// The port number the server will run on

int createUDPServer() {
	struct sockaddr_in srvAddr;
	int server;

	if ((server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) displayErrMsg("Socket CreationFailed");	// Create a UDP socket, if this fails display an error message
	else printf("\nSocket Created\n");

	memset((char *) &srvAddr, 0, sizeof(srvAddr));

	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(HANGMAN_UDP_PORT);
	srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(server, (struct sockaddr *) &srvAddr, sizeof(srvAddr)) == -1) displayErrMsg("Bind Failed");
	printf("\nSocket Binding Completed\n");

	return server;	
}

int createUDPClient(char* server_name){
	struct sockaddr_in srvAddr;
	int client;

	if ((client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)  displayErrMsg("Socket Failed");
	else printf("\nSocket Created\n");

	return client;
}

struct sockaddr_in getServerAddress(char* server_name){
	struct sockaddr_in srvAddr;

	memset((char *) &srvAddr, 0, sizeof(srvAddr));

	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(HANGMAN_UDP_PORT);

	if (inet_aton(server_name, &srvAddr.sin_addr) == 0 )  {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	return srvAddr;
}

