 /* 
	File: 		servf.c
	Version: 	Modified version of Hangman Server
	Author:		Joe O'Regan

	Year 4 Networked Games Assignment

	Network server for hangman game with fork()

	Concurrent Hangman Server: handles multiple clients at a time by forking a child copy of the server for each new client connection
	The Unix fork function is the simplest technique for a concurrent server, creating one cheild process for each client.

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690
	
	18/11/2017	Separated play_hangman functionality to TCPPlayHangman.h
	14/10/2017	Added separate files to handle creating the socket, and displaying the address and port number
	08/10/2017	Added forking capability for server. 
			Game is exactly the same for each client, so moved random number seeding to while loop
			Displays the word the client was guessing on the server side
	23/09/2017 	Added includes to get rid of compile warnings (wasn't necessary)
			Client IP and Port number is displayed when a connection is made
			Win/Lose message displayed, and sent to Client to display
			Added hangman graphic
*/

#include <stdio.h>									// printf()
#include <string.h>									// 23/09/2017 Warning for strcpy, bzero()
#include <stdlib.h>									// 23/09/2017 Warning for exit()
#include <unistd.h>									// 23/09/2017 gethostname(), write(), read(), close()
#include <sys/wait.h>									// 08/10/2017 wait(), waitpid(), signal(), SIGCHLD
#include "../DrawHangman.h"								// Display the hangman graphics
#include "../Hangman.h"									// Hangman game functions
#include "../CreateTCPSocket.h"								// Create a TCP Server Socket
#include "../TCPPlayHangman.h"								// 18/11/2017 Moved hang_man() function to separate file

extern time_t time ();									// Seed the random number
char cliName[INET_ADDRSTRLEN];								// Client address string
struct sockaddr_in client;								// Client address structure
#define CLI_PORT ntohs(client.sin_port)							// The port number the client is using to connect to the server

void main () {										// No command line arguments
	int sock, fd;									// Socket, client socket, and length of client socket
	socklen_t client_len;								// int type in sys/socket.h, socklen_t: unsigned opaque integral type of length of at least 32 bits

	/* FORK VARIABLES */
	pid_t childpid;									// pid_t: signed int data type, capable of representing  a process ID. 
	void sig_chld(int);								// Signal handling function, catches SIGCHLD signal from terminating process

	sock = createTCPServerSocket();							// Functionality to create socket moved to separate file CreateTCPServer.h
	// The UNP books signal() function calls POSIX sigaction
	// signal(<signal name>, <point to signal handler function>);
	signal(SIGCHLD, sig_chld);							// SIGCHLD signal is sent to the parent of a child process when it exits, is interuppted, or resumes after interruption

	drawHangman();									// Draw the hangman graphic

 	while (1) {									// Infinite loop
 		client_len = sizeof (client);						// Size of the client socket
 		if ((fd = accept (sock, (struct sockaddr *) &client, &client_len)) < 0)	// P41 A new descriptor is returned by accept() for each client that connects to our server
			displayErrMsgStatus("Accepting Connection", 3);			// Display the error message, and exit with status 3

		/* DISPLAY CLIENT ADDRESS AND PORT */
		displayNameAndPort(client, cliName);					// Display the Client IP and Port number

		/* RANDOM NUMBER SEED - DIFFERENT WORD FOR EACH CLIENT CONNECTION */
		srand(time(NULL));							// 08/10/2017 Seed/Reseed the random number. Moved to while loop so each client receives a different word

		/*--- FORK - CREATE NEW CHILD PROCESS TO HANDLE THE CLIENT CONNECTION ----
			The original server forks child process copies itself to handle incoming clients
			Each child process has its own process ID
			fork returns 0 for a child, and the childs process ID for the parent 
		*/
		if ( (childpid = fork()) == 0) {					// Creating child process of Server to handle client. Assigning unique process ID to the child.
			close(sock);							// close listening socket
			playHangmanTCP(fd, fd, cliName, CLI_PORT);			// Play the game - playHangmanTCP() located in TCPPlayHangman.h
			exit(0);							// Process termination
		}

 		close (fd);								// Close the connection to client when the game has finished playing
 	}
}


/*----------------------------------- HANDLE ZOMBIES -----------------------------------*/
/*	P180 Zombies - Signal handler to catch SIGCHLD 
	(SIGCHLD default disposition is to be ignored)
	Needs to be done before we fork the first child, and needs to be done once
	P185 5.11 Correct version of sig_child calls waitpid
*/
void sig_chld(int signo) {								// Signal handler catches SIGCHLD signal from terminating process
	pid_t pid;									// The child process ID
	int stat;									// The status of the child

	//pid = wait(&stat);								// Call to wait() handles the terminated child
	//printf("Child %d has terminated\n", pid);					// Diagnostic tool - Shows when the child terminates
	
	// Handle zombies, preventing them from being left around, 
	// waitpid can be called in a loop, wait can't, no way to stop wait from blocking
	// A SIGCHLD handler must be coded correctly using waitpid, 
	// to prevent any zombies from being left around
	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)					// Calling wait insufficient for preventing zombies, WNOHANG: tells waitpid not to block if child running that has not terminated.
		printf("Child %d terminated\n", pid);					// Display the process ID of the terminated child
	
	return;										// return not necessary here (void) - Reminder that return may interrupt a system call
}
