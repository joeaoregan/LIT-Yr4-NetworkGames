#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <arpa/inet.h>

void displayAddress(const struct sockaddr* address, FILE* stream){
	// Test for address and stream
	if (address == NULL || stream == NULL) return;				// return from function if the struct or file is empty
	
	void *numericAddress;							// Pointer to binary address
	char addressBuffer[INET6_ADDRSTRLEN];					// string large enough to hold IPv6 address
	in_port_t port;								// Port to display

	if (address->sa_family == AF_INET) {					// If the address family is IPv4
		numericAddress = &((struct sockaddr_in *) address)->sin_addr;	// Get the IPv4 address to display
		port = ntohs(((struct sockaddr_in *) address)->sin_port);	// Get the port number to display
	}
	else if (address->sa_family == AF_INET6) {				// if the address family is IPv6
		numericAddress = &((struct sockaddr_in6 *) address)->sin6_addr;	// Get the IPv6 address to display
		port = ntohs(((struct sockaddr_in6 *) address)->sin6_port);	// Get the port number to display
	}
	else {
		fputs("Stream error", stream);					// Error with stream
		return;
	}
	
	// Convert binary to printable address
	if (inet_ntop(address->sa_family, numericAddress, addressBuffer, sizeof(addressBuffer)) == NULL)
		fputs("[invalid address]", stream); 				// Unable to convert
	else {
		fprintf(stream, "%s", addressBuffer);
		if (port != 0) fprintf(stream, "/%u", port);			// Zero not valid in socket address
	}
}
