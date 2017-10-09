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
#include <stdio.h>
#include <syslog.h>
#include <signal.h>
#include <errno.h>
#include <string.h>										// 23/09/2017 Warning for strcpy
#include <stdlib.h>										// 23/09/2017 Warning for exit
#include <unistd.h>										// 23/09/2017 gethostname(), write(), read(), close()
#include <arpa/inet.h>										// 23/09/2017 inet_ntop()
#include <sys/wait.h>										// 08/10/2017 wait()
#include <time.h>

extern time_t time ();

int maxlives = 12;
char *word [] = {
#include "words"
};
# define NUM_OF_WORDS (sizeof (word) / sizeof (word [0]))
# define MAXLEN 80 										/* Maximum size in the world of Any string */
# define HANGMAN_TCP_PORT 1066									// The port number

char clntName[INET_ADDRSTRLEN];									// Client address string
struct sockaddr_in server, client;
char *hangman[]= {										// ASCII hangman graphic
"   T E A M 1",											// Changed to Team 1 (the actual team name)
" H A N G M A N",
"  _____________", 
"  |/      |",
"  |   ___(\")___",
"  |      |_|",
"  |      /^\\",
"  |    _// \\\\_",
"__|____________"};    

 void play_hangman(int in, int out);								// Function delcaration for play_hangman

 void main ()											// No command line arguments
 {
 	int sock, fd, client_len;								// Socket, client socket, and length of client socket
	//socklen_t client_len;
	pid_t childpid;										// pid_t: signed int data type, capable of representing  a process ID
	socklen_t clilen;									// socklen_t: unsigned opaque integral type of length of at least 32 bits
	struct sockaddr_in cliaddr, servaddr;							// Internet socket address structures
	void sig_chld(int);

 	//srand ((int) time ((long *) 0)); 							// 08-10-2017 Moved - randomize the seed - Same Game Each Client

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

	// Draw hangman
	for (int h = 0; h < 9; ++h){								// Draw hangman ASCII
	  printf("%s\n", hangman[h]);
	}

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
 	char * whole_word, part_word [MAXLEN],							// The word to guess, the word so far
 	guess[MAXLEN], outbuf [MAXLEN];

 	int lives = maxlives;									// Initialise the number of guesses
 	int game_state = 'I';									// I = Incomplete
 	int i, good_guess, word_length;								// Iterator, correct guesses, 
 	char hostname[MAXLEN];									// Name of the current system

 	gethostname (hostname, MAXLEN);								// Get the host name
	
 	//sprintf(outbuf, "Playing hangman on host: %s \n \n", hostname);
 	snprintf(outbuf, sizeof(outbuf), "Playing hangman on host: %s \n \n", hostname);	// Set outbuf to server hostname, with protection against buffer overflow
 	write(out, outbuf, strlen (outbuf));							// Send outbuf to client

 	/* Pick a word at random from the list */
 	whole_word = word[rand() % NUM_OF_WORDS];						// Select random word from words array
 	word_length = strlen(whole_word);							// Get length of word
 	syslog (LOG_USER | LOG_INFO, "server chose hangman word %s", whole_word);		// Message logging

 	/* No letters are guessed Initially */
 	for (i = 0; i <word_length; i++)
 		part_word[i]='-';								// Display hyphens to indicate size of word
 	
	part_word[i] = '\0';									// Add string terminating character

 	//sprintf (outbuf, "%s %d \n", part_word, lives);					// Set outbuf to the part word & the lives left
 	snprintf (outbuf, sizeof(outbuf), "%s %d \n", part_word, lives);			// Convert to text string. Set outbuf to the part word & the lives left, with protection against buffer overflow
 	write (out, outbuf, strlen(outbuf));							// Send to client

 	while (game_state == 'I')								// Loop until game is complete
 	/* Get a letter from player guess */
 	{
		while (read (in, guess, MAXLEN) <0) {						// Read guess from client
 			if (errno != EINTR)							// Check for error
 				exit (4);
 			printf ("re-read the startin \n");
 			} 									/* Re-start read () if interrupted by signal */
 		good_guess = 0;									// Set/reset good_guess to false
 		for (i = 0; i <word_length; i++) {
 			if (guess [0] == whole_word [i]) {					// If the guess from client is a match
 				good_guess = 1;							// Set good guess true
 				part_word [i] = whole_word [i];					// Fill the guessed letter in correct position
 			}
 		}
 		if (! good_guess) lives--;							// If the player guesses wrong, decrement lives
 		if (strcmp (whole_word, part_word) == 0)					// If all letters guessed correctly
 			game_state = 'W'; 							/* W ==> User Won */
 		else if (lives == 0) {								// If client has run out of guesses
 			game_state = 'L'; 							/* L ==> User Lost */
 			strcpy (part_word, whole_word); 					/* User Show the word */
 		}
 		//sprintf (outbuf, "%s %d \n", part_word, lives);				// Set the part_word and lives to be sent
 		snprintf (outbuf, sizeof(outbuf), "%s %d \n", part_word, lives);		// Convert to text string. Set the part_word and lives to be sent, with protection against buffer overflow
 		write (out, outbuf, strlen (outbuf));						// Send outbuf info to client
	
		// Send game over message
		if (game_state == 'W') {
			printf("Client %s/%d has guessed \"%s\" correctly!\n", clntName, ntohs(client.sin_port), whole_word);
 			//sprintf (outbuf, "\nPlayer Has Guessed \"%s\" Correctly!\n", whole_word);			// Set win message to return to client
 			snprintf (outbuf, sizeof(outbuf), "\nPlayer Has Guessed \"%s\" Correctly!\n", whole_word);	// Set win message to return to client
 			write (out, outbuf, strlen (outbuf));								// Send outbuf info to client
		}
		else if (game_state == 'L') {
			printf("Client %s/%d is a loser!\n", clntName, ntohs(client.sin_port));
 			//sprintf (outbuf, "\nPlayer Has Run Out Of Guesses!\n");					// Set lose message to return to client
 			snprintf (outbuf, sizeof(outbuf), "\nPlayer Has Run Out Of Guesses!\n");			// Set lose message to return to client, with protection against buffer overflow
 			write (out, outbuf, strlen (outbuf));								// Send outbuf info to client
		}
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
