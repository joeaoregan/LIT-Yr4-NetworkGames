/* 
 	File: 		Hangclient.c
	Version: 	IPv6 Client for hangman server
	Modified by: 	Jason Foley

	Year 4 Networked Games Assignment

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690
*/

 #include <stdio.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <netdb.h>
 #include <arpa/inet.h>
 #include <unistd.h>
 #include <stdllib.h>
 #include <string.h>
 #include <unistd.h> // read(), write()

 # define LINESIZE 80
 # define HANGMAN_TCP_PORT 1066

 int main (int argc, char * argv [])
 {
 	struct sockaddr_in6 server; /* Server's address assembled here */
 	struct hostent * host_info;
 	int sock, count;
 	 char i_line[LINESIZE];
 	 char o_line[LINESIZE];
 	char * server_name;

 	/* Get server name from the command line.  If none, use 'localhost' */

 	server_name = (argc = 1)?  argv [1]: "localhost";

 	/* Create the socket */
 	sock = socket (AF_INET6, SOCK_STREAM, 0);
 	if (sock <0) {
 		perror ("Creating stream socket");
 		exit (1);
 	}

 	host_info = gethostbyname2(server_name,AF_INET6);
 	if (host_info == NULL) {
 		fprintf (stderr, "%s: unknown host: %s \n", argv [0], server_name);
 		exit (2);
 	}

 	/* Set up the server's socket address, then connect */

 	server.sin6_family = AF_INET6;
 	//inet_pton ((char *) & server.sin6_addr, host_info->h_addr, host_info->h_length);
  inet_pton (AF_INET6, "::1",(char *) & server.sin6_addr);
 	server.sin6_port = htons (HANGMAN_TCP_PORT);

 	if (connect (sock, (struct sockaddr *) & server, sizeof server)) {
 		perror ("connecting to server");
 		exit (3);
 	}
 	/*OK connected to the server.  
 Take a line from the server and show it, take a line and send the user input to the server. 
 Repeat until the server terminates the connection. */

 	printf ("Connected to server %s \n", server_name);
 	while ((count = read (sock, i_line, LINESIZE)) > 0) {
 		write (1, i_line, count);
 		count = read (0, o_line, LINESIZE);//0 = STDIN
 		write (sock, o_line, count);
 	}
 }
