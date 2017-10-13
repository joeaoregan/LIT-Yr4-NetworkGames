 /* 
	Network server for hangman game

 	File: hangserver.c

	Concurrent Hangman Server: handles multiple clients at a time by forking a child copy of the server for each new client connection
	The Unix fork function is the simplest technique for a concurrent server, creating one cheild process for each client.

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690

	08/10/2017	Added forking capability for server. 
			Game is exactly the same for each client, so moved random number seeding to while loop
			Displays the word the client was guessing on the server side
	23/09/2017 	Added includes to get rid of compile warnings (wasn't necessary)
			Client IP and Port number is displayed when a connection is made
			Win/Lose message displayed, and sent to Client to display
			Added hangman graphic
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>										// printf()
#include <syslog.h>										// syslog()
#include <signal.h>
#include <errno.h>
#include <string.h>										// 23/09/2017 Warning for strcpy, bzero()
#include <stdlib.h>										// 23/09/2017 Warning for exit
#include <unistd.h>										// 23/09/2017 gethostname(), write(), read(), close()
#include <arpa/inet.h>										// 23/09/2017 inet_ntop()
#include <sys/wait.h>										// 08/10/2017 wait()
#include <time.h>
#include "../DrawHangman.h"
#include "../Hangman.h"

extern time_t time ();
char clntName[INET_ADDRSTRLEN];									// Client address string
struct sockaddr_in server, client;
#define CLI_PORT ntohs(client.sin_port)

void main ()											// No command line arguments
{
	int sock, fd, client_len;								// Socket, client socket, and length of client socket
	//socklen_t client_len;
	pid_t childpid;										// pid_t: signed int data type, capable of representing  a process ID
	socklen_t clilen;									// socklen_t: unsigned opaque integral type of length of at least 32 bits
	//struct sockaddr_in cliaddr, servaddr;							// Internet socket address structures
	void sig_chld(int);

 	sock = socket (AF_INET, SOCK_STREAM, 0);						// 0 or IPPROTO_TCP	// Create the socket

 	if (sock < 0) { 									// This error checking is the code Stevens wraps in his Socket Function etc
 		perror ("creating stream socket");
 		exit (1);
 	}

	bzero(&server, sizeof(server));								// Set the entire address structure to 0, easier to remember than memset
 	server.sin_family = AF_INET;								// IPv4 address
 	server.sin_addr.s_addr = htonl(INADDR_ANY);						// Server IP
 	server.sin_port = htons(HANGMAN_TCP_PORT);						// Server port

 	if (bind(sock, (struct sockaddr *) & server, sizeof(server)) < 0) {			// socket(), bind(), listen() -> Server prepared to accept connection
 		perror ("binding socket");
	 	exit (2);
 	}

 	listen (sock, 5);									// socket(), bind(), listen() -> server passive open

	signal(SIGCHLD, sig_chld);								// SIGCHLD signal is sent to the parent of a child process when it exits, is interuppted, or resumes after interruption

	drawHangman();										// Draw the hangman graphic

	printf("Server now running on port: %d \n", ntohs(server.sin_port));			// Display port number

 	while (1) {
 		client_len = sizeof (client);							// Size of the client socket
 		if ((fd = accept (sock, (struct sockaddr *) &client, &client_len)) <0) {	// P41 A new descriptor is returned by accept() for each client that connects to our server
 			perror ("accepting connection");
 			exit (3);
 		}

		// Display Client address and port
		if (inet_ntop(AF_INET, &client.sin_addr.s_addr, clntName,sizeof(clntName)) != NULL){
  			printf("Handling client %s/%d \n", clntName, ntohs(client.sin_port));
		}

		srand(time(NULL));								// 08/10/2017 Seed/Reseed the random number

		if ( (childpid = fork()) == 0) {						// child process, was Fork()
			close(sock);								// close listening socket, was Close()
			//srand(time(NULL));							// Test random seed
 			play_hangman (fd, fd);							// Play the game
			exit(0);								// Process termination
		}

 		close (fd);									// Close the connection to client
 	}
 }

 /* ---------------- Play_hangman () ---------------------*/

 void play_hangman (int in, int out)
 {
 	char * whole_word, part_word [LINESIZE],
 	guess[LINESIZE], outbuf [LINESIZE];
 	int i, word_length, lives = MAX_LIVES, game_state = 'I';					// Iterator, length of whole_word, correct guesses, game state
 	
  	displayHostname(out, outbuf);									// Display the name of the server

 	whole_word = whole_word = selectRandomWord(clntName, CLI_PORT);					// Select random word from words array
 	word_length = strlen(whole_word);								// Get length of word

 	/* No letters are guessed Initially */
 	for (i = 0; i <word_length; i++)
 		part_word[i]='-';									// Display hyphens to indicate size of word
 	
	part_word[i] = '\0';										// Add string terminating character

 	sprintf (outbuf, "%s %d \n", part_word, lives);							// Set outbuf to the part word & the lives left
 	write (out, outbuf, strlen(outbuf));								// Send to client

 	while (game_state == 'I')									// Loop until game is complete
 	/* Get a letter from player guess */
 	{
		while (read (in, guess, LINESIZE) <0) {							// Read guess from client
 			if (errno != EINTR)								// Check for error
 				exit (4);
 			printf ("re-read the startin \n");
 		} 											/* Re-start read () if interrupted by signal */

    		if (!correctGuess(whole_word, part_word, guess)) lives--;				// If it's an incorrect guess, decrement the number of lives

 		if (strcmp (whole_word, part_word) == 0)						// If all letters guessed correctly
 			game_state = 'W'; 								/* W ==> User Won */
 		else if (lives == 0) {									// If client has run out of guesses
 			game_state = 'L'; 								/* L ==> User Lost */
 			strcpy (part_word, whole_word); 						/* User Show the word */
 		}
 		sprintf (outbuf, "%s %d \n", part_word, lives);						// Set the part_word and lives to be sent
 		write (out, outbuf, strlen (outbuf));							// Send outbuf info to client
	
    		checkGameOver(game_state, outbuf, whole_word, out, clntName, CLI_PORT); // If game is finished, display win/lose message
 	}
 }

// P180 Zombies - Signal handler to catch SIGCHLD (SIGCHLD default disposition is to be ignored)
// Needs to be done before we fork the first child, and needs to be done once
void sig_chld(int signo) {
	pid_t pid;
	int stat;

	//pid = wait(&stat);				// Call to wait() handles the terminated child
	//printf("Child %d has terminated\n", pid);	// Diagnostic tool - Shows when the child terminates
	
	// Handle zombies, preventing them from being left around, waitpid can be called in a loop, wait can't
	// A SIGCHLD handler must be coded correctly using waitpid, to prevent any zombies from being left around
	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)	// Calling wait insufficient for preventing zombies, WNOHANG: tells waitpid not to block if child running that has not terminated. wait can't be called in a loop, no way to stop wait from blocking
		printf("Child %d terminated\n", pid);
	
	return;						// return not necessary here (void) - Reminder that return may interrupt a system call
}
