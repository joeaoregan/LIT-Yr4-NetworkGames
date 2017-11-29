 /* 
 	File: 		serveruf.c
	Version: 	Hangman UDP Concurrent Server Using Fork()
	Modified by:	Joe O'Regan

	Year 4 Networked Games Assignment

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690

	25/11/2017	Added option to specify port as a command line parameter
*/

#include <stdio.h>
#include <string.h>										// strlen()
#include <stdlib.h>										// exit(), rand(), srand()
#include <unistd.h>										// write(), read(), close()
#include <sys/wait.h>										// wait(), waitpid(), signal(), SIGCHLD
#include <time.h>										// time(), Seed the random number
#include "../DrawHangman.h"									// Draw graphics and add colour, needs to be included before Hangman.h
#include "../Hangman.h"										// Functions to play hangman
#include "../CreateUDPSocket.h"									// Functions to create and use UDP sockets
#include "../Socket.h"										// Socket functions intended for use with both TCP and UDP; displayHostname()
#include "../UDPPlayHangman.h"									// Functions to play hangman using sendto() and recvfrom() (include after Socket.h)

struct sockaddr_in client;									// Client address structure

int main (int argc, char * argv []) {								// Option to specify port as a command line parameter
	int sock, cliSock, count;								// Client and server socket, and byte count variables
	char username[LINESIZE];								// Buffer string, the username entered by the Client
	socklen_t cliLen;									// int type in sys/socket.h, socklen_t: unsigned opaque integral type (min. length 32 bits)
	cliLen = sizeof(client);								// Size of the client socket

	/* FORK VARIABLES */
	pid_t childpid;										// pid_t: signed int data type, capable of representing  a process ID. 
	void sig_chld(int);									// Signal handling function, catches SIGCHLD signal from terminating process

	sock = createUDPServer((argc == 2) ? argv[1] : UDP_PORT);				// Create the UDP Server socket, with port specified, or default 1068

	signal(SIGCHLD, sig_chld);								// SIGCHLD signal sent to parent of a child process when it exits, 
												// is interupted, or resumes after interruption
	drawHangmanLogo(UDP_FORK);								// DrawHangman.h: Draw the game logo, with socket type label

	while(1) {										// Infinite loop
//		fflush(stdout);									// flushes the output buffer of a stream
		srand(time(NULL));								// Randomise the seed, to get a different word each game
count = recvfrom(sock,username,LINESIZE,0,(struct sockaddr*)&client,&cliLen);
//		if ((count = recvfrom(cliSock,username,LINESIZE,0,(struct sockaddr*)&client,&cliLen)) > 0) {// If the bytes received > 0 a connection was made
		if (count > 0) {// If the bytes received > 0 a connection was made

			username[count-1] = '\0';						// Terminate the end of the string (before the '\n' new line character)
			printf("Username received: %s%s%s\n",BLUE,username,NORM);		// Format and display the username

			childpid = fork();							// Creating child process of Server to handle client. Assigning unique process ID to child.
			if(childpid == 0) {							// fork() returns 0 from child process
				close(sock);							// Close listening socket, after forking a child process
				playHangmanUDP(sock,sock,username);				// Play the game using the username entered to identify the player

				printf("Finished Playing Hangman for %s%s%s\n",BLUE,username,NORM);	// Inform on server side the game has finished
				close(cliSock);								// Finished with client
				exit(0);								// Process termination
			}
		}
	
		close(cliSock);									// Close the socket connection
	}	
}

/*
	Function to handle Zombie Processes
*/
void sig_chld(int signo) {						// Signal handler catches SIGCHLD signal from terminating process
	pid_t pid;							// The child process ID
	int stat;							// The status of the child

	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)			// Calling wait insufficient for preventing zombies, WNOHANG: tells waitpid not to block
									// if child running that has not terminated.
		printf("Child %s%d%s terminated\n", BLUE,pid,NORM);	// Display the process ID of the terminated child
	
	return;								// return not necessary here (void) - Reminder that return may interrupt a system call
}
