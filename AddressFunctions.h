 /* 
 	File: 		AddressFunctions.h
	Version: 	Functions to display the IP of client and server sockets
	Author:		Joe O'Regan

	Year 4 Networked Games Assignment

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690


	Code adapted (unsuccessfully) from: 
	TCP/IP Sockets in C, Donahoo, Calvert -> PrintSocketAddr() P42
	Unix Network Programming, Stevens, Fenner, Rudoff
	Beej's Guide to Network Programming, Hall
*/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <arpa/inet.h>


int get_ip_str(int ip, void *sa, char *s, size_t maxlen);


/* 
	Server: Display the IP address and Port the Server is running on 
*/
void displayAddress(const struct sockaddr* address, FILE* stream){					// Test for address and stream	
	if (address == NULL || stream == NULL) return;							// return from function if the struct or file is empty
	
	void *numericAddress;										// Pointer to binary address
	char addressBuffer[INET6_ADDRSTRLEN];								// string large enough to hold IPv6 address
	in_port_t port;											// Port to display

	if (address->sa_family == AF_INET) {								// If the address family is IPv4
		numericAddress = &((struct sockaddr_in *) address)->sin_addr;				// Get the IPv4 address to display
		port = ntohs(((struct sockaddr_in *) address)->sin_port);				// Get the port number to display
	}
	else if (address->sa_family == AF_INET6) {							// if the address family is IPv6
		numericAddress = &((struct sockaddr_in6 *) address)->sin6_addr;				// Get the IPv6 address to display
		port = ntohs(((struct sockaddr_in6 *) address)->sin6_port);				// Get the port number to display
	}
	else {
		fputs("Stream error", stream);								// Error with stream
		return;
	}

	fputs("Server now running on ", stdout);  							// Convert binary to printable address
        if(get_ip_str(address->sa_family, (void *) address, addressBuffer, INET6_ADDRSTRLEN) == 1) {
		fputs("[invalid address]", stream); 							// Unable to convert
	} else {r
		printf("%s", addressBuffer);
		if (port != 0) fprintf(stream, "/%u", port);						// Zero not valid in socket address
	}

      fputc('\n', stdout);										// Add new line
}

/* 
	Function to return the IP address for an IPv4 or IPv6 address structure. 
	Taken from beej website, but not implemented 
*/
int get_ip_str(int ip, void *sa, char *s, size_t maxlen) {
    switch(ip) {
        case AF_INET:											// If it's an IPv4 address set the address value in the IPv4 address structure
            	inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr), s, maxlen); 
	 	break;		
        case AF_INET6:											// If it's an IPv6 address set the address value in the IPv6 address stucture
            	inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr), s, maxlen); 	
	    	break;
        default:
            	strncpy(s, "Unknown Address Family", maxlen);						// If it's neither, something is wrong
            	return 1;										// if unsuccessful 
    }
    return 0;												// successful
}

char* storeAndDisplayIP(struct sockaddr_in addr, char* ip, int port) {					// Store and display the client IP and Port 	
	if (inet_ntop(AF_INET, &addr.sin_addr.s_addr, ip, INET_ADDRSTRLEN) != NULL){			// inet_ntop() Convert IP address to human readable form
		printf("Handling client %s/%d \n", ip, port);						// Display the client IP address and port number
	}	
	printf("storeAndDisplayIP: %s\n", ip);
	return ip;
}
