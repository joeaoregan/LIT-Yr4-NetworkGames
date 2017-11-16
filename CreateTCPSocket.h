/*
	CreateTCPServer.h

	Reconfigured version of SetupTCPServerSocket() function from TCPServerUtility.c
	from TCP/IP Sockets in C book, to work with TCP Hangman fork() server

	16/11/2017 Added HandleErrors.h for error handling
*/

#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include "Hangman.h"
#include "AddressFunctions.h"
#include "HandleErrors.h"								// Display error messages

#define SRV_IP "127.0.0.1"								// IPv4 Address of server on local machine
#define TCP_PORT_NUM 1066								// The port number the server will run on
#define	MAX_CLIENTS 5									// 2nd argument to listen(), the maximum number of client connections
char* TCP_PORT = "1066";								// The port number the server will run on, for createTCPServerSocket() function

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
  if (rtnVal != 0)    
    printf("getaddrinfo() failed %s\n", gai_strerror(rtnVal));				// DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnVal));

  int server = -1;									// Initialise the server socket
  for (struct addrinfo *addr = servAddr; addr != NULL; addr = addr->ai_next) {
    // Create a TCP socket
    server = socket(servAddr->ai_family, servAddr->ai_socktype, servAddr->ai_protocol);
    if (server < 0) continue;       							// Socket creation failed; try next address

    if ((bind(server, servAddr->ai_addr, servAddr->ai_addrlen) == 0) &&			// Bind to the local address 
      (listen(server, MAX_CLIENTS) == 0)) {						// Set socket to listen
    
      // Print local address of socket
      struct sockaddr_storage localAddr;
      socklen_t addrSize = sizeof(localAddr);
      if (getsockname(server, (struct sockaddr *) &localAddr, &addrSize) < 0) 
		displayErrMsg("getsockname() failed");

      fputs("Server now running on ", stdout);
      displayAddress((struct sockaddr *) &localAddr, stdout);				// Display the address/port for the socket
      fputc('\n', stdout);								// Add new line

      break;       									// Bind and list successful
    }

    close(server);  									// Close and try again
    server = -1;
  }

  // Free address list allocated by getaddrinfo()
  freeaddrinfo(servAddr);								// frees the memory that was allocated for the dynamically allocated linked list 

  return server;
}

struct sockaddr_in createTCPClientSocket(int* sock, char* server_name) {		/* CREATE THE SOCKET */
	struct sockaddr_in servAddr;
 	struct hostent * host_info;
 	
	 /* CREATE THE SOCKET */
 	(*sock) = socket(AF_INET, SOCK_STREAM, 0);
 	if (sock < 0) displayErrMsgStatus("Creating Stream Socket", 1);	

 	host_info = gethostbyname(server_name);
 //	if (host_info == NULL) {
 //	  fprintf (stderr, "%s: unknown host:%s \n", argv [0], server_name);
 //	  exit (2);
 //	}

/* SET UP THE SERVER'S SOCKET ADDRESS, AND CONNECT */
 	servAddr.sin_family = host_info->h_addrtype;
 	memcpy ((char *) & servAddr.sin_addr, host_info->h_addr, host_info->h_length);
 	servAddr.sin_port = htons(TCP_PORT_NUM);

 	if (connect ((*sock), (struct sockaddr *) &servAddr, sizeof servAddr) < 0)		// If the connection fails
		displayErrMsgStatus("Connecting To Server", 3);				// Display the error message, and exit with status 3

 	printf ("Connected to server: %s \n", server_name);	

	return servAddr;
}
