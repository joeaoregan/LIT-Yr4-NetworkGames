 /* 
 	File: 		CreateTCPSocket.h
	Version: 	Functions to create and connect TCP sockets
	Author:		Joe O'Regan

	Year 4 Networked Games Assignment

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690

	Reconfigured version of SetupTCPServerSocket() function from TCPServerUtility.c
	from TCP/IP Sockets in C book, to work with TCP Hangman fork() server

	16/11/2017 Added HandleErrors.h for error handling
*/

#include <sys/socket.h>									// sockaddr, accept(), bind(), listen()
#include <unistd.h>									// gethostname(), write(), read(), close()
#include <arpa/inet.h>									// inet_ntop()
#include <netdb.h>									// gethostbyname()
#include <string.h>									// strcpy(), bzero()
#include "Hangman.h"
#include "AddressFunctions.h"
#include "HandleErrors.h"								// Display error messages
#include "Socket.h"

#define	max(a,b) ((a) > (b) ? (a) : (b))						// Return the largest of 2 numbers

#define SRV_IP "127.0.0.1"								// IPv4 Address of server on local machine
#define TCP_PORT_NUM 1066								// The port number the server will run on
#define	MAX_CLIENTS 5									// 2nd argument to listen(), the maximum number of client connections
char* TCP_PORT = "1066";								// The port number the server will run on, for createTCPServerSocket() function


/*
	SERVER:
	Function to create a server-side TCP socket that can handle both IPv4 & IPv6
	client connections. Taking a parameter of the port number to run the Server on
*/
int createDualStackServerSocket(char* port){
	struct sockaddr_in6 server;
	int i, sock, reuseaddr = 1;	
	char* s;
	//char str[INET6_ADDRSTRLEN];

	bzero(&server, sizeof server);							// Zero out structure, could also use memset(), swapping 2 args in bzero will be caught by compiler

	sock = socket(AF_INET6, SOCK_STREAM, 0);					// Create an IPv6 socket
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof reuseaddr);	// Set the options for the socket
	if (sock < 0) displayErrMsgStatus("Creating Stream Socket", 1);			// HandleErrors.h: Error creating the socket, socket() returns -1 if there is an error
	
 	server.sin6_family = AF_INET6;							// IPv6 socket
 	server.sin6_addr = in6addr_any;							// Store an IPv6 Address
 	server.sin6_port = htons(atoi(port));						// The TCP port to listen for connections on

 	if (bind(sock, (struct sockaddr *) & server, sizeof(server)) < 0) 		// Bind - Assign the address to the socket
		displayErrMsgStatus("Binding Socket", 1);				// bind() returns -1 if there is an error
	if (listen(sock, MAX_CLIENTS) < 0)						// Set socket to listen
		displayErrMsgStatus("Listen()", 1);					// listen() returns -1 if there is an error
	else {		
		displayHostIPPort(atoi(port));						// Socket.h: Display the local machine Name, IP, and port the server is running on

		//inet_ntop(AF_INET6, &(server.sin6_addr),str,INET6_ADDRSTRLEN);	// display IPv6 address
		//printf("%s\n", str);
		//s = get_ip_str((struct sockaddr *)server,s,INET_ADDRSTRLEN);
	}

	return sock;									// Return the socket back to the server application
}


/*
	SERVER:
	Function to create a Server socket using the TCP protocol
	Taking a parameter of the port number to run the Server on
*/
int createTCPServerSocket(char* port) {
  // Construct the server address structure
  struct addrinfo addrCriteria;                   					// Criteria for address match
  memset(&addrCriteria, 0, sizeof(addrCriteria)); 					// Zero out structure
  addrCriteria.ai_family = AF_UNSPEC;             					// Any address family
  addrCriteria.ai_flags = AI_PASSIVE;             					// Accept on any address/port
  addrCriteria.ai_socktype = SOCK_STREAM;         					// Only stream sockets
  addrCriteria.ai_protocol = IPPROTO_TCP;         					// Only TCP protocol

  struct addrinfo *servAddr; 								// List of server addresses
  int rtnVal = getaddrinfo(NULL, port, &addrCriteria, &servAddr);			// Specify port number or use default 1066, address structure, 
  if (rtnVal != 0) printf("getaddrinfo() failed %s\n", gai_strerror(rtnVal));		// DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnVal));

  int server = -1;									// Initialise the server socket
  for (struct addrinfo *addr = servAddr; addr != NULL; addr = addr->ai_next) {   
    server = socket(servAddr->ai_family, servAddr->ai_socktype, servAddr->ai_protocol); // Create a TCP socket
    if (server < 0) continue;       							// Socket creation failed; try next address

    if ((bind(server, servAddr->ai_addr, servAddr->ai_addrlen) == 0) &&			// Bind to the local address 
      (listen(server, MAX_CLIENTS) == 0)) {						// Set socket to listen
    
      // Print local address of socket
      struct sockaddr_storage localAddr;						// Address structure capable of holding IPv4 and IPv6 addresses
      socklen_t addrSize = sizeof(localAddr);						// Size of the address stucture
      if (getsockname(server, (struct sockaddr *) &localAddr, &addrSize) < 0) 		// Get the local IP address
		displayErrMsg("getsockname() failed");

	displayHostIPPort(atoi(port));							// Socket.h: Display the local machine Name, IP & port the server is running on

      break;       									// Bind and list successful
    }

    close(server);  									// Close and try again
    server = -1;									// Reset the socket
  }

  freeaddrinfo(servAddr);								// Frees memory getaddrinfo() allocated for the dynamically allocated linked list

  return server;									// Return the socket
}


/*
	CLIENT:
	Function to create a Client socket using the TCP protocol
	Can use default IP and Port numbers, or parameters from command line
*/
struct sockaddr_in createTCPClientSocket(int* sock, char* server_name, int port) {	/* CREATE THE SOCKET */
	struct sockaddr_in servAddr;
 	struct hostent * host_info;							// Used to get the host name
	char* err;
 	
	 /* CREATE THE SOCKET */
 	(*sock) = socket(AF_INET, SOCK_STREAM, 0);					// AF_INET = Address Family Internet, SOCK_STREAM = streams
 	if (sock < 0) displayErrMsgStatus("Creating Stream Socket", 1);	

 	host_info = gethostbyname(server_name);						// Replaced by getaddrinfo(), used for DNS lookups
//	if (host_info == NULL) {
//		sprintf(err, "Unkown Host %s", server_name);
//		displayErrMsgStatus("", 2);
//	}

/* SET UP THE SERVER'S SOCKET ADDRESS, AND CONNECT */
	bzero(&servAddr, sizeof(servAddr));						// Zero out the address

	//servaddr.sin_family = AF_INET;						// Set to IPv4 address family
 	servAddr.sin_family = host_info->h_addrtype;					// Get the address family from the gethostbyname() results
 	memcpy ((char *) & servAddr.sin_addr, host_info->h_addr, host_info->h_length);	
 	servAddr.sin_port = htons(port);						// htons() host to network short: convert port number to network byte order

 	if (connect ((*sock), (struct sockaddr *) &servAddr, sizeof servAddr) < 0)	// If the connection fails
		displayErrMsgStatus("Connecting To Server", 3);				// HandleErrors.h: Display the error message, and exit with status 3

 	printf ("Connected to server: %s \n", server_name);	

	return servAddr;								// Return the server address structure
}


/* 
	SERVER:
	Intended to display the Client's IP address and port
	after connecting to the server, using getpeername()
	Using a sockaddr_storage address structure both IPv4 and IPv6 can be displayed
*/
void displayIPAndPort(struct sockaddr_storage* cli, int clilen , char* name, int* sock, int* port) {	// XXX added size of address structure, fixes some issues 
	getpeername((*sock), (struct sockaddr*) &(*cli), &clilen);			// Get the foreign address, the address of the socket connecting to you

	// Handle both IPv4 and IPv6:
	if ((*cli).ss_family == AF_INET) {						// AF_INET is the IPv4 address family
		struct sockaddr_in *s = (struct sockaddr_in *) &(*cli);			// Convert to IPv4 address structure
		(*port) = ntohs(s->sin_port);						// Set the port number
		inet_ntop(AF_INET, &s->sin_addr, name, INET_ADDRSTRLEN);
		printf("IPv4 Connection Received ");
	} else { 									// Otherwise AF_INET6 is IPv6 address family
		struct sockaddr_in6 *s = (struct sockaddr_in6 *) &(*cli);		// Convert to IPv4 address structure
		(*port) = ntohs(s->sin6_port);						// Set the port number
		inet_ntop(AF_INET6, &s->sin6_addr, name, INET6_ADDRSTRLEN);
		printf("IPv6 Connection Received ");
	}

	printf("Handling client %s/%d\n", name, (*port));
}


int createTCPClientDualStack(char* serverIP, int port){
	int proto = 4;
	struct addrinfo hints;
	struct addrinfo* servinfo;  		// will point to the results

	memset(&hints, 0, sizeof hints); 	// make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     	// don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; 	// TCP stream sockets
	hints.ai_flags = AI_PASSIVE;     	// fill in my IP for me	


	int rtnVal = getaddrinfo(serverIP, "1066", &hints, &servinfo);			// Specify port number or use default 1066, address structure, 
//	if (rtnVal != 0) printf("getaddrinfo() failed %s\n", gai_strerror(rtnVal));	// DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnVal));
	
	if(servinfo->ai_family == AF_INET) {
		printf("Creating IPv4 Socket Connection To: %s/%d\n", serverIP,port);
	} else {
		printf("Creating IPv6 Socket Connection To: %s/%d\n", serverIP,port);
		proto = 6;
	}

	freeaddrinfo(servinfo); 		// free the linked-list

	return proto;
}


void IPv4TCPClientSocket(int* sock, char* server_name, int port) {			// Create a Socket using an IPv4 address structure
	struct sockaddr_in servAddr;							// Assemble Server's address using IPv4 address structure
 	struct hostent * host_info;							// Used to get the host name
	char* err;
 	
	 /* CREATE THE SOCKET */
 	(*sock) = socket(AF_INET, SOCK_STREAM, 0);					// AF_INET = Address Family Internet, SOCK_STREAM = streams
 	if (sock < 0) displayErrMsgStatus("Creating Stream Socket", 1);	

 	host_info = gethostbyname(server_name);						// Replaced by getaddrinfo(), used for DNS lookups
	if (host_info == NULL) displayErrMoreSpecific("Unknown Host",2,server_name);	// HandleErrors: more descriptive error message, with string appends

/* SET UP THE SERVER'S SOCKET ADDRESS, AND CONNECT */
	bzero(&servAddr, sizeof(servAddr));						// Zero out the address

	//servaddr.sin_family = AF_INET;						// Set to IPv4 address family
 	servAddr.sin_family = host_info->h_addrtype;					// Get the address family from the gethostbyname() results
 	memcpy ((char *) & servAddr.sin_addr, host_info->h_addr, host_info->h_length);	
 	servAddr.sin_port = htons(port);						// htons() host to network short: convert port number to network byte order

 	if (connect ((*sock), (struct sockaddr *) &servAddr, sizeof servAddr) < 0)	// If the connection fails
		displayErrMsgStatus("Connecting To Server", 3);				// HandleErrors.h: Display the error message, and exit with status 3
							// Assemble Server's address using IPv6 address structure
 	printf ("Connected to server: %s \n", server_name);	
}


void IPv6TCPClientSocket(int* sock, char* server_name, int port) {			// Create a Socket using an IPv4 address structure
	struct sockaddr_in6 servAddr;							// Assemble Server's address using IPv6 address structure
 	
	 /* CREATE THE SOCKET */
 	(*sock) = socket(AF_INET6, SOCK_STREAM, 0);					// AF_INET = Address Family Internet, SOCK_STREAM = streams
 	if (sock < 0) displayErrMsgStatus("Creating Stream Socket", 1);	

	/* SET UP THE SERVER'S SOCKET ADDRESS, AND CONNECT */
	bzero(&servAddr, sizeof(servAddr));						// Zero out the address

 	servAddr.sin6_family = AF_INET6;						// Set to IPv6 address family  
	inet_pton(AF_INET6, server_name,(char *) & servAddr.sin6_addr);
 	servAddr.sin6_port = htons(port);						// htons() host to network short: convert port number to network byte order

 	if (connect ((*sock), (struct sockaddr *) &servAddr, sizeof servAddr) < 0)	// If the connection fails
		displayErrMsgStatus("Connecting To Server", 3);				// HandleErrors.h: Display the error message, and exit with status 3

 	printf ("Connected to server: %s \n", server_name);	
}
