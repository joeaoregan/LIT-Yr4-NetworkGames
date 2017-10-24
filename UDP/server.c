/* 
	server.c

	UDP server socket
	
	25/10/2017

	All compile warnings removed, with necessary includes added, and dangerous functions replaced

	To stop buffers displaying gibberish when using recvfrom
	Have to null terminate buffer strings received with recvfrom with '\0'

	fgets better to use than gets() for keyboard input, need to state size of the buffer
	Can set to 1 character for hangman game

	need to cast sockaddr_in to sockaddr stucture for bind, sendto, and recvfrom
*/

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>			// exit()
#include "HandleErrors.h"		// Error handling functions header file
#include "../Hangman.h"			// Include Hangman game header file from parent directory

struct sockaddr_in si_me, si_other;
int slen = sizeof(si_other);

int main(void) {
	int sock, i;// slen=sizeof(si_other);
	char buf[LINESIZE];
	int byteCount;

	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) displayErrMsg("socket");	// create UDP socket

	memset((char *) &si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(HANGMAN_TCP_PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	printf("Waiting for connections...\n");

	if (bind(sock, (struct sockaddr *) &si_me, sizeof(si_me)) == -1) displayErrMsg("bind");
	//printf("bind\n");

	//while(1) {
		//play_hangman(sock, sock);

		//if (recvfrom(sock, buf, LINESIZE, 0, &si_other, &slen)==-1) displayErrMsg("recvfrom()");
		//printf("Guess from %s:%d\nLetter: %c\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), buf[0]);


		//if (recvfrom(sock, buf, LINESIZE, 0, &si_other, &slen) == -1) displayErrMsg("recvfrom()");
		byteCount = recvfrom(sock, buf, LINESIZE, 0, (struct sockaddr *) &si_other, &slen);
		//printf("Guess from %s:%d\nReceived: %s\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), buf);
		printf("Guess from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		//printf("Received: %s\n\n", buf);
		buf[byteCount-1] = '\0'; 					// Need to null-terminate received data (TCP/IP Sockets in C chapter 4: P60/55)
		printf("Received %d bytes: %s", byteCount, buf);
		//write(1, buf, byteCount);
		printf("\n");


// SEND
		if (sendto(sock, buf, strlen(buf), 0, (struct sockaddr *) &si_other, slen) == -1) displayErrMsg("sendto()");

		close(sock);
	//}
	return 0;
}

/* ---------------- Play_hangman () ---------------------*/

void play_hangman (int in, int out) {



	printf("play_hangman\n");


 	char * whole_word, part_word [LINESIZE],
 	guess[LINESIZE], outbuf[LINESIZE];

 	int lives = MAX_LIVES;								// Initialise the number of guesses to 12
 	int game_state = 'I';								// Game State: I = Incomplete
 	int i, good_guess, word_length;
 	char hostname[LINESIZE];

 	gethostname (hostname, LINESIZE);
 	sprintf(outbuf, "Playing hangman on host %s: \n \n", hostname);			
 	write(out, outbuf, strlen (outbuf));						// Display client details message serves side
 	
 	whole_word = word[rand() % NUM_OF_WORDS];					// Pick a word at random from the list
 	word_length = strlen(whole_word);
 	syslog (LOG_USER | LOG_INFO, "Server chose hangman word %s", whole_word);


	printf("Word chosen: %s\n", whole_word);

 	
 	for (i = 0; i < word_length; i++) part_word[i] = '-';				// Set part-word to string of '-' No letters are guessed Initially
 	
	part_word[i] = '\0';								// Terminate the part-word string



 	sprintf (outbuf, "%s %d \n", part_word, lives);					// Merge the part-word and number of guesses left to a string
 	//write (out, outbuf, strlen(outbuf));						
	sendto(out, outbuf, LINESIZE, 0, (struct sockaddr *) &si_other, slen);				// and send it to the client

	


	if (recvfrom(in, guess, LINESIZE, 0, (struct sockaddr *) &si_other, &slen) == -1) displayErrMsg("recvfrom()");
	printf("Guess from %s:%d\nLetter: %c\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), guess[0]);
	//if (recvfrom(sock, buf, LINESIZE, 0, &si_other, &slen)==-1) displayErrMsg("recvfrom()");
	//printf("Guess from %s:%d\nLetter: %c\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), buf[0]);

/*

 	while (game_state == 'I') { 							// Get a letter from player guess
// READ
		//while (read (in, guess, LINESIZE) <0) {
 		//	if (errno != EINTR)
 		//		exit (4);
 		//	printf ("re-read the startin \n");
 		//} 									// Re-start read () if interrupted by signal
		if (recvfrom(in, guess, LINESIZE, 0, &si_other, &slen) == -1) displayErrMsg("recvfrom()");
		printf("Guess from %s:%d\nLetter: %c\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), guess[0]);




	 	good_guess = 0;

	 	for (i = 0; i < word_length; i++) {					// For every character in the word
	 		if (guess [0] == whole_word[i]) {				// If a part-word letter matches a whole word letter
	 		good_guess = 1;							// It is a good guess
	 		part_word [i] = whole_word[i];					// Add the letter to the part-word
	 		}
	 	}



	 	if (! good_guess) lives--;						// If its not a good guess, decrement the number of lives

	 	if (strcmp (whole_word, part_word) == 0) game_state = 'W'; 		// W ==> User Won
	 	else if (lives == 0) {
	 		game_state = 'L'; 						// L ==> User Lost
	 		strcpy (part_word, whole_word); 				// User Show the word
	 	}


// SEND
	 	//sprintf (outbuf, "%s %d \n", part_word, lives);
	 	//write (out, outbuf, strlen (outbuf));
	 	sprintf (outbuf, "%s %d \n", part_word, lives);				// Merge the part-word and number of guesses left to a string
		sendto(out, outbuf, LINESIZE, 0, &si_other, slen);			// and send it to the client
 	}
*/
 }
