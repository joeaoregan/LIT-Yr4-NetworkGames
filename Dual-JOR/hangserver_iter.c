 /* Network server for hangman game */
 /* File: hangserver.c */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <syslog.h>
#include <signal.h>
#include <arpa/inet.h>
#include <errno.h>

#include <stdlib.h> // XXX
#include <string.h> // XXX
#include <unistd.h> // XXX read(), write()

#include "../DrawHangman.h"
#include "../CreateTCPSocket.h"	// XXX

 extern time_t time ();

 int maxlives = 12;
 //char *word [] = {
 //	#include "../words"						// XXX ../ = go up one folder to find that file
 //};
 #define NUM_OF_WORDS (sizeof (word) / sizeof (word [0]))
 #define MAXLEN 80 /* Maximum size in the world of Any string */
 #define HANGMAN_TCP_PORT 1066

void play_hangman (int in, int out); 					// XXX void return type

int main ()  {								// XXX int return type, add "return 0" at end
 	int sock, fd, client_len;
	//int reuseaddr = 1;
 	struct sockaddr_in6 server, client;

 	srand ((int) time ((long *) 0)); 						/* randomize the seed */

/*
 	sock = socket(AF_INET6, SOCK_STREAM, 0);					//0 or IPPROTO_TCP
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));	// IPV6_V6ONLY turned off allows dual-stack sockets
 	if (sock <0) { 									//This error checking is the code Stevens wraps in his Socket Function etc
 		perror ("creating stream socket");
 		exit (1);
 	}

 	server.sin6_family = AF_INET6;
 	server.sin6_addr = in6addr_any;
 	server.sin6_port = htons(HANGMAN_TCP_PORT);

 	if (bind(sock, (struct sockaddr *) & server, sizeof(server)) <0) {
 		perror ("binding socket");
	 	exit (2);
 	}

 	listen (sock, 5);
*/
	sock = createDualStackServerSocket();	// XXX Moved socket create, bind, listen, functionality to CreateTCPSocket.h

 	while (1) {
 		client_len = sizeof (client);
 		if ((fd = accept (sock, (struct sockaddr *) &client, &client_len)) <0) {
 			perror ("accepting connection");
 			exit(3);
 		}
 		play_hangman (fd, fd);
 		close (fd);
 	}

	return 0; // XXX
 }

 /* ---------------- Play_hangman () ---------------------*/

void play_hangman (int in, int out)
{
 	char * whole_word, part_word [MAXLEN],
 	guess[MAXLEN], outbuf [MAXLEN];

 	int lives = maxlives;
 	int game_state = 'I';//I = Incomplete
 	int i, good_guess, word_length;
 	char hostname[MAXLEN];

 	gethostname (hostname, MAXLEN);
 	sprintf(outbuf, "Playing hangman on host %s: \n\n", hostname); // XXX
 	write(out, outbuf, strlen (outbuf));

 	/* Pick a word at random from the list */
 	whole_word = word[rand() % NUM_OF_WORDS];
 	word_length = strlen(whole_word);
 	syslog (LOG_USER | LOG_INFO, "server chose hangman word %s", whole_word);

 	/* No letters are guessed Initially */
 	for (i = 0; i <word_length; i++)
 		part_word[i]='-';
 	
	part_word[i] = '\0';

 	sprintf (outbuf, "%s %d \n", part_word, lives);
 	write (out, outbuf, strlen(outbuf));

 	while (game_state == 'I')
 	/* Get a letter from player guess */
 	{
		while (read (in, guess, MAXLEN) <0) {
 			if (errno != EINTR)
 				exit (4);
 			printf ("re-read the startin \n");
 		} 						/* Re-start read () if interrupted by signal */
	 	good_guess = 0;
	 	for (i = 0; i <word_length; i++) {
	 		if (guess [0] == whole_word [i]) {
	 		good_guess = 1;
	 		part_word [i] = whole_word [i];
	 		}
	 	}
	 	if (! good_guess) lives--;
	 	if (strcmp (whole_word, part_word) == 0)
	 		game_state = 'W'; 			/* W ==> User Won */
	 	else if (lives == 0) {
	 		game_state = 'L';			/* L ==> User Lost */
	 		strcpy (part_word, whole_word); 	/* User Show the word */
	 	}

	 	sprintf (outbuf, "%s %d \n", part_word, lives);
	 	write (out, outbuf, strlen (outbuf));
 	}
 }
