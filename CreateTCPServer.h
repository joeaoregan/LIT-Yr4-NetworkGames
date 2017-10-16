/*
	CreateTCPServer.h

	Reconfigured version of SetupTCPServerSocket() function from TCPServerUtility.c
	from TCP/IP Sockets in C book, to work with TCP Hangman fork() server
*/

#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include "Hangman.h"
#include "AddressFunctions.h"

int createTCPServerSocket(const char *port) {
  // Construct the server address structure
  struct addrinfo addrCriteria;                   					// Criteria for address match
  memset(&addrCriteria, 0, sizeof(addrCriteria)); 					// Zero out structure
  addrCriteria.ai_family = AF_UNSPEC;             					// Any address family
  addrCriteria.ai_flags = AI_PASSIVE;             					// Accept on any address/port
  addrCriteria.ai_socktype = SOCK_STREAM;         					// Only stream sockets
  addrCriteria.ai_protocol = IPPROTO_TCP;         					// Only TCP protocol

  struct addrinfo *servAddr; 								// List of server addresses
  int rtnVal = getaddrinfo(NULL, port, &addrCriteria, &servAddr);
  if (rtnVal != 0)    
    printf("getaddrinfo() failed %s\n", gai_strerror(rtnVal));				// DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnVal));

  int server = -1;									// Initialise the server socket
  for (struct addrinfo *addr = servAddr; addr != NULL; addr = addr->ai_next) {
    // Create a TCP socket
    server = socket(servAddr->ai_family, servAddr->ai_socktype, servAddr->ai_protocol);
    if (server < 0) continue;       							// Socket creation failed; try next address

    if ((bind(server, servAddr->ai_addr, servAddr->ai_addrlen) == 0) &&			// Bind to the local address 
        (listen(server, MAX_CLIENTS) == 0)) {						// Set socket to listen
    
//    	printf("Server now running on port: %s \n", port);				// Previous code to display port number

      // Print local address of socket
      struct sockaddr_storage localAddr;
      socklen_t addrSize = sizeof(localAddr);
      if (getsockname(server, (struct sockaddr *) &localAddr, &addrSize) < 0)
 //       DieWithSystemMessage("getsockname() failed");
		printf("getsockname() failed");
      fputs("Server now running on ", stdout);
      displayAddress((struct sockaddr *) &localAddr, stdout);				// Display the address/port for the socket
      fputc('\n', stdout);
      break;       									// Bind and list successful
    }

    close(server);  									// Close and try again
    server = -1;
  }

  // Free address list allocated by getaddrinfo()
  freeaddrinfo(servAddr);								// frees the memory that was allocated for the dynamically allocated linked list 

  return server;
}
