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
	client connections
*/
int createDualStackServerSocket(){
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
 	server.sin6_port = htons(TCP_PORT_NUM);						// The TCP port to listen for connections on

 	if (bind(sock, (struct sockaddr *) & server, sizeof(server)) < 0) 		// Bind - Assign the address to the socket
		displayErrMsgStatus("Binding Socket", 1);				// bind() returns -1 if there is an error
	if (listen(sock, MAX_CLIENTS) < 0)						// Set socket to listen
		displayErrMsgStatus("Listen()", 1);					// listen() returns -1 if there is an error
	else {		
		displayHostIPPort(TCP_PORT_NUM);					// Socket.h: Display the local machine Name, IP, and port the server is running on

		//inet_ntop(AF_INET6, &(server.sin6_addr),str,INET6_ADDRSTRLEN);	// display IPv6 address
		//printf("%s\n", str);
		//s = get_ip_str((struct sockaddr *)server,s,INET_ADDRSTRLEN);
	}

	return sock;									// Return the socket back to the server application
}


/*
	SERVER:
	Function to create a Server socket using the TCP protocol
*/
int createTCPServerSocket() {
  // Construct the server address structure
  struct addrinfo addrCriteria;                   					// Criteria for address match
  memset(&addrCriteria, 0, sizeof(addrCriteria)); 					// Zero out structure
  addrCriteria.ai_family = AF_UNSPEC;             					// Any address family
  addrCriteria.ai_flags = AI_PASSIVE;             					// Accept on any address/port
  addrCriteria.ai_socktype = SOCK_STREAM;         					// Only stream sockets
  addrCriteria.ai_protocol = IPPROTO_TCP;         					// Only TCP protocol

  struct addrinfo *servAddr; 								// List of server addresses
  int rtnVal = getaddrinfo(NULL, TCP_PORT, &addrCriteria, &servAddr);
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

      displayAddress((struct sockaddr *) &localAddr, stdout);				// AddressFunctions.h: Display the address/port for the socket

      break;       									// Bind and list successful
    }

    close(server);  									// Close and try again
    server = -1;									// Reset the socket
  }

  // Free address list allocated by getaddrinfo()
  freeaddrinfo(servAddr);								// frees the memory that was allocated for the dynamically allocated linked list 

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
 	
	 /* CREATE THE SOCKET */
 	(*sock) = socket(AF_INET, SOCK_STREAM, 0);					// AF_INET = Address Family Internet, SOCK_STREAM = streams
 	if (sock < 0) displayErrMsgStatus("Creating Stream Socket", 1);	

 	host_info = gethostbyname(server_name);						// Replaced by getaddrinfo(), used for DNS lookups
 //	if (host_info == NULL) {
 //	  fprintf (stderr, "%s: unknown host:%s \n", argv [0], server_name);
 //	  exit (2);
 //	}

/* SET UP THE SERVER'S SOCKET ADDRESS, AND CONNECT */
	bzero(&servAddr, sizeof(servAddr));						// Zero out the address

	//servaddr.sin_family = AF_INET;
 	servAddr.sin_family = host_info->h_addrtype;					// Get the address family from the gethostbyname() results
 	memcpy ((char *) & servAddr.sin_addr, host_info->h_addr, host_info->h_length);	
//	servAddr.sin_port = htons(TCP_PORT_NUM);					// htons() host to network short: convert port number to network byte order
 	servAddr.sin_port = htons(port);						// htons() host to network short: convert port number to network byte order
	//inet_pton(AF_INET, (argc == 1) ?  SRV_IP : argv[1], &servaddr.sin_addr);

 	if (connect ((*sock), (struct sockaddr *) &servAddr, sizeof servAddr) < 0)	// If the connection fails
		displayErrMsgStatus("Connecting To Server", 3);				// HandleErrors.h: Display the error message, and exit with status 3

 	printf ("Connected to server: %s \n", server_name);	

	return servAddr;								// Return the server address structure
}


/* 
	SERVER:
	Intended to display the Client's IP address and port
	after connecting to the server, using getpeername()
*/
char* displayNameAndPort(struct sockaddr_in* cli, char* name, int sock) {
	int clilen = sizeof(cli);						

	getpeername(sock, (struct sockaddr*) &cli, &clilen);				// Get the foreign address, the address of the socket connecting to you
//	char name[INET_ADDRSTRLEN];							// Client address string
	//inet_pton(AF_INET, "127.0.0.1", &(client.sin_addr));
	//if (inet_ntop(AF_INET, &cli.sin_addr.s_addr, name,sizeof(name)) != NULL){	// sizeof(name) not working here, INET_ADDRSTRLEN is the length of an address string
	if (inet_ntop(AF_INET, &(*cli).sin_addr.s_addr, name, INET_ADDRSTRLEN) != NULL){// Convert the address to a string, and store in clntName
		printf("Handling client %s/%d\n", name, ntohs((*cli).sin_port));	// Display the client IP address and port number, ntohs = convert from network byte order to host byte order
	}

	return name;									// Return the IP address
}
