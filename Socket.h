 /* 
 	File: 		Socket.h
	Version: 	Intended to hold common functionality for TCP and UDP sockets
	Modified by:	Joe O'Regan

	Year 4 Networked Games Assignment

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690

	22/11/2017 	Modified displayHostname() to only take a socket
			Declaring it's own instance of a buffer
*/

#ifndef	__SOCKET_H
#define	__SOCKET_H


/*
	SERVER:
	Display the name of the local machine using gethostname()
*/
void displayHostname() {
	char hostname[LINESIZE], buf[LINESIZE];					// Name of the current system

	gethostname(hostname, LINESIZE);					// Get the host name for the local machine
	
	snprintf(buf, LINESIZE, "Playing hangman on host: %s\n\n", hostname);	// Set outbuf to server hostname, with protection against buffer overflow
	write(0, buf, strlen(buf));						// Display hostname on server side
}


/*
	SERVER TCP & UDP:
	Use gethostname() to display the name of the local machine and get gethostbyname() 
	to get the local IP address using the host name. 
	Taking the port number as a parameter and displaying it.
*/
void displayHostIPPort(int port) {
	int i;								// Index for for loope, to create address
	struct hostent * host_info;					// Can only return IPv4 addresses
	char hostname[LINESIZE];					// Hostname for the local machine
	struct in_addr **addr_list;

	if (gethostname(hostname, sizeof hostname) == 0) {		// gethostname() returns 0 on success, gets computer name the socket is running on
		printf("Running Hangman Server on: %s ", hostname);	// Display the name of the local machine

		host_info = gethostbyname(hostname);			// gethostbyname() uses the name of the machine to return the local IP address
		addr_list = (struct in_addr **) host_info->h_addr_list;	// list of ip addresses

		for (i = 0; addr_list[i] != NULL; i++) 			// Search through the list of IP addresses
			printf("%s", inet_ntoa(*addr_list[i]));		// inet_ntoa() converts the address to human readable format (deprecated: inet_ntop())
		printf(":%d\n",port);					// Display the port number
	}
}

/*
// Beej: Display the IP and Port of the connected socket
void displayPeerIPAndPort(int sock, struct sockaddr_storage client, int client_len){
	char ipstr[INET6_ADDRSTRLEN];
	int port;

	getpeername(sock, (struct sockaddr*) &client, &client_len);
		// deal with both IPv4 and IPv6:
		if (client.ss_family == AF_INET) {
		    printf("IPv4 Address Used By Client\n");
		    struct sockaddr_in *s = (struct sockaddr_in *)&client;
		    port = ntohs(s->sin_port);
		    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
		} else if (client.ss_family == AF_INET6) { // AF_INET6
		    printf("IPv6 Address Used By Client\n");
		    struct sockaddr_in6 *s = (struct sockaddr_in6 *)&client;
		    port = ntohs(s->sin6_port);
		    inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
		}

		printf("Peer IP address: %s\n", ipstr);
		printf("Peer port      : %d\n", port);
}
*/

#endif	/* __SOCKET_H */
