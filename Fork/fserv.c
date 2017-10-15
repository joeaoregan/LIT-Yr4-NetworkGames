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
#include "../CreateTCPServer.h"

extern time_t time ();										// Seed the random number
char clntName[INET_ADDRSTRLEN];									// Client address string
struct sockaddr_in server, client;
#define CLI_PORT ntohs(client.sin_port)

char* service = "1066";

void main ()											// No command line arguments
{
	int sock, fd, client_len;								// Socket, client socket, and length of client socket
	//socklen_t client_len;									// int type in sys/socket.h that is at least 32 bits
/* FORK VARIABLES */
/**/	pid_t childpid;										// pid_t: signed int data type, capable of representing  a process ID. 
/**/	socklen_t clilen;									// socklen_t: unsigned opaque integral type of length of at least 32 bits
	//struct sockaddr_in cliaddr, servaddr;							// Internet socket address structures
/**/	void sig_chld(int);									// Signal handling function, catches SIGCHLD signal from terminating process

/**/ 	//srand ((int) time ((long *) 0)); 							// Randomize the seed - Moved inside while loop, so each client connection generates a different whole_word

//	sock = createTCPServerSocket((char*)HANGMAN_TCP_PORT);
	sock = createTCPServerSocket(TCP_PORT);
/*
 	sock = socket (AF_INET, SOCK_STREAM, 0);						// 0 or IPPROTO_TCP	// Create the socket

 	if (sock < 0) { 									// This error checking is the code Stevens wraps in his Socket Function etc
 		perror ("creating stream socket");
 		exit (1);
 	}

*/
/**//*	bzero(&server, sizeof(server));								// Set the entire address structure to 0, easier to remember than memset, seems to work fine without this
 	server.sin_family = AF_INET;								// IPv4 address
 	server.sin_addr.s_addr = htonl(INADDR_ANY);						// Server IP
 	server.sin_port = htons(HANGMAN_TCP_PORT);						// Server port

 	if (bind(sock, (struct sockaddr *) & server, sizeof(server)) < 0) {			// socket(), bind(), listen() -> Server prepared to accept connection
 		perror ("binding socket");
	 	exit (2);
 	}

 	listen (sock, 5);									// socket(), bind(), listen() -> server passive open
*/	
	// signal(<signal name>, <point to signal handler function>);
	signal(SIGCHLD, sig_chld);								// SIGCHLD signal is sent to the parent of a child process when it exits, is interuppted, or resumes after interruption

	drawHangman();										// Draw the hangman graphic

//	printf("Server now running on port: %d \n", ntohs(server.sin_port));			// Display port number

 	while (1) {
 		client_len = sizeof (client);							// Size of the client socket
 		if ((fd = accept (sock, (struct sockaddr *) &client, &client_len)) <0) {	// P41 A new descriptor is returned by accept() for each client that connects to our server
 			perror ("accepting connection");
 			exit (3);
 		}

/* DISPLAY CLIENT ADDRESS AND PORT */
/**/		if (inet_ntop(AF_INET, &client.sin_addr.s_addr, clntName,sizeof(clntName)) != NULL){
/**/  			printf("Handling client %s/%d \n", clntName, ntohs(client.sin_port));
/**/		}

/* RANDOM NUMBER SEED - DIFFERENT WORD FOR EACH CLIENT CONNECTION */
/**/		srand(time(NULL));								// 08/10/2017 Seed/Reseed the random number. Moved to while loop so each client receives a different word

/* FORK - CREATE NEW CHILD PROCESS TO HANDLE THE CLIENT CONNECTION */
/* 	The original server forks child process copies itself to handle incoming clients
	Each child process has its own process ID
	fork returns 0 for a child, and the childs process ID for the parent */
/**/		if ( (childpid = fork()) == 0) {						// Creating child process of Server to handle client. Assigning unique process ID to the child.
/**/			close(sock);								// close listening socket
/**/			//srand(time(NULL));							// Test random seed - didn't work
 			play_hangman (fd, fd);							// Play the game
/**/			exit(0);								// Process termination
/**/		}
 		close (fd);									// Close the connection to client when the game has finished playing
 	}
}

/* ---------------- Play_hangman () ---------------------*/

void play_hangman (int in, int out)
{
/**/	char * whole_word, part_word [LINESIZE],						// LINESIZE is declared in Hangman.c replacing MAXLEN
/**/	guess[LINESIZE], outbuf [LINESIZE];
 	int i, word_length, lives = MAX_LIVES, game_state = 'I';				// Iterator, length of whole_word, correct guesses, game state
 	
/**/  	displayHostname(out, outbuf);								// Function in Hangman.h: Display the name of the server

/* RANDOM WORD SELECTION FUNCTION */
/**/ 	whole_word = whole_word = selectRandomWord(clntName, CLI_PORT);				// Function in Hangman.h: Select random word from words array
 	word_length = strlen(whole_word);							// Get length of word

 	/* No letters are guessed Initially */
 	for (i = 0; i <word_length; i++)
 		part_word[i]='-';								// Display hyphens in place of letters, to indicate size of word
 	
	part_word[i] = '\0';									// Add string terminating character

 	sprintf (outbuf, "%s %d \n", part_word, lives);						// Set outbuf to the part word & the lives left, creating a string to send to the client
 	write (out, outbuf, strlen(outbuf));							// Send outbuf string to client

 	while (game_state == 'I')								// Loop until game is complete
 	/* Get a letter from player guess */
 	{
/**/		while (read (in, guess, LINESIZE) <0) {						// Read guess from client
 			if (errno != EINTR)							// P182 Check for EINTR error if stuck in slow system call, and exit if restart signal not recieved
 				exit (4);
 			printf ("re-read the startin \n");
 		} 										// Re-start read () if interrupted by signal

/* CHECK GUESS FUNCTION */
/**/   		if (!correctGuess(whole_word, part_word, guess)) lives--;			// Functionality moved to Hangman.h. Incorrect guess: decrement number of lives. Good guess: copy the letter to the part word

 		if (strcmp (whole_word, part_word) == 0)					// If all letters guessed correctly
 			game_state = 'W'; 							// W ==> User Won 
 		else if (lives == 0) {								// If client has run out of guesses
 			game_state = 'L'; 							// L ==> User Lost
 			strcpy (part_word, whole_word); 					// User Show the word
 		}
 		sprintf (outbuf, "%s %d \n", part_word, lives);					// Set the part_word and lives to be sent
 		write (out, outbuf, strlen (outbuf));						// Send outbuf info to client

/* DISPLAY WIN/LOSE MESSAGE */	
/**/   		checkGameOver(game_state, outbuf, whole_word, out, clntName, CLI_PORT); 	// Hangman.h function: If game is finished, display win/lose message
 	}
 }

/* HANDLE ZOMBIES */
/*	P180 Zombies - Signal handler to catch SIGCHLD 
	(SIGCHLD default disposition is to be ignored)
	Needs to be done before we fork the first child, and needs to be done once
	P185 5.11 Correct version of sig_child calls waitpid
*/
void sig_chld(int signo) {									// Signal handler catches SIGCHLD signal from terminating process
	pid_t pid;
	int stat;

	//pid = wait(&stat);									// Call to wait() handles the terminated child
	//printf("Child %d has terminated\n", pid);						// Diagnostic tool - Shows when the child terminates
	
	// Handle zombies, preventing them from being left around, 
	// waitpid can be called in a loop, wait can't
	// A SIGCHLD handler must be coded correctly using waitpid, 
	// to prevent any zombies from being left around
	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)						// Calling wait insufficient for preventing zombies, WNOHANG: tells waitpid not to block if child running that has not terminated.
		printf("Child %d terminated\n", pid);						// wait() can't be called in a loop, no way to stop wait from blocking
	
	return;											// return not necessary here (void) - Reminder that return may interrupt a system call
}
