 /* 
	Network server for hangman game

 	File: hangclient.c

	Client for hangman server.

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690
	
	17/10/2017 	Added includes to remove warning messages
*/

 #include <stdio.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <netdb.h>
#include <string.h>				// 17/10/2017 Warning for strcpy, bzero()
#include <stdlib.h>				// 17/10/2017 Warning for exit
#include <unistd.h>				// 17/10/2017 gethostname(), write(), read(), close()

 # define LINESIZE 80
 # define MAXLEN 80
 # define HANGMAN_TCP_PORT 1066

 int main (int argc, char * argv [])
 {
 	struct sockaddr_in server; /* Server's address assembled here */
 	struct hostent * host_info;
 	int sock, count, fd;
	char buffer[1024];
 	char i_line[LINESIZE];
 	char o_line[LINESIZE];
 	char * server_name;

 	/* Get server name from the command line.  If none, use 'localhost' */

 	server_name = (argc = 1)?  argv [1]: "localhost";

 	/* Create the socket */
 	sock = socket (PF_INET, SOCK_DGRAM, 0);
 	if (sock <0) {
 		perror ("Creating stream socket");
 		exit (1);
 	}

 	host_info = gethostbyname(server_name);
 	if (host_info == NULL) {
 		fprintf (stderr, "%s: unknown host:%s \n", argv [0], server_name);
 		exit (2);
 	}

 	/* Set up the server's socket address, then connect */

 	server.sin_family = host_info->h_addrtype;
 	memcpy ((char *) & server.sin_addr, host_info->h_addr, host_info->h_length);
 	server.sin_port = htons (HANGMAN_TCP_PORT);

	fd = buffer + 1;	// What's this about Jason? Gives compile warning

	int n;
	char sendline[MAXLEN];

 	if (sendto (sock, sendline, strlen(sendline), 0,(struct sockaddr *) & server, sizeof server) <0) {
 		perror ("connecting to server");
 		exit (3);
 	}
 	/*OK connected to the server.  
 Take a line from the server and show it, take a line and send the user input to the server. 
 Repeat until the server terminates the connection. */

 	printf ("Connected to server%s \n", server_name);
 	while ((count = read (sock, i_line, LINESIZE)) > 0) {
 		write (1, i_line, count);
 		count = read (0, o_line, LINESIZE);//0 = STDIN
 		write (sock, o_line, count);
 	}
 }
