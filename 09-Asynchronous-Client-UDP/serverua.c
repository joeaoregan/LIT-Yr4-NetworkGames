/* 	
	Joe O'Regan K00203642
	17/11/2017

	serverua.c

	Test Asynchronous UDP Client

	20171117 	Asynchronous UDP client sends and receives, and terminates when finished
	20171116 	CreateUDPSocket.h - abstracts the UDP socket creation code
	20171115 	Fixed client continuing to connect
	20171114 	Joe: Fixed warning messages by casting sockaddr_in to struct sockaddr*
			And moved HandleErrors to parent folder	
			Added random word selection	
*/

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>							// Seed the random number
#include "../DrawHangman.h"
#include "../Hangman.h"
#include "../CreateUDPSocket.h"

struct sockaddr_in cliAddr;

int slen = sizeof(cliAddr);
void play_hangman (int in, int out);

int main (int argc, char * argv []) {					// Option to specify port as a command line parameter
	int sock;
	char buf[LINESIZE];

	sock = createUDPServer((argc == 2) ? argv[1] : UDP_PORT);	// Create the UDP Server socket, with port specified, or default 1068

	drawHangman();

	printf("\nAsynchronous Test Server: Waiting For Data...\n");

	fflush(stdout);
	srand(time(NULL));

	play_hangman(sock,sock);

	close(sock);	

	return 0;
}

/* ---------------- Play_hangman () ---------------------*/
void play_hangman (int in, int out) {
	printf("\nBegin Playing\n");

 	char * whole_word, part_word[LINESIZE], guess[LINESIZE], outbuf[LINESIZE];
 	int lives = MAX_LIVES, game_state = 'I', i, good_guess, word_length;
 	char hostname[LINESIZE];
	ssize_t byteCount;

 	gethostname (hostname, LINESIZE);
//	sprintf(outbuf, "Playing hangman on host %s: \n", hostname);			
 //	write(0, outbuf, strlen(outbuf));					// XXX	 
	printf("Playing hangman on host %s: \n", hostname);
/*RECEIVE USERNAME*/
	if((byteCount = recvfrom(in, guess, LINESIZE, 0, (struct sockaddr *) &cliAddr, &slen)) == 0) {	// Server receives 1st
		displayErrMsg("recvfrom() Failed");
	}		
	printf("byteCount: %ld\n", byteCount);
	guess[byteCount-1] = '\0';
	printf("Username received: %s\n",guess);
/*GET RANDOM WORD*/
	whole_word = selectRandomWord("Client", 0000);				// XXX FIX THIS LATER - CLIENT IP AND PORT
 	word_length = strlen(whole_word);

 	
 	for (i = 0; i < word_length; i++) {
		part_word[i] = '-';
	}
	part_word[i] = '\0';
	guess[0] = '\0';

	part_word[i] = '\0';
	outbuf[0] = '\0';
	printf("sendto 1 outbuf before sprintf: %sXX\n", outbuf);
 	sprintf (outbuf, "%s %d\n", part_word, lives);	
	sendto(out, outbuf, strlen(outbuf), 0, (struct sockaddr*) &cliAddr, sizeof cliAddr);	

	while (game_state == 'I') {	
		if ((byteCount = recvfrom(in, guess, LINESIZE, 0, (struct sockaddr *) &cliAddr, &slen)) == -1){	// cast sockaddr_in to sockaddr
			 displayErrMsg("recvfrom() Failed");
		}

		guess[1] = '\0';										// only need 1st character
		printf("Guess: %s ByteCount: %ld\n", guess, byteCount);						// Show what was received from the client

 		good_guess = 0;

 		for (i = 0; i < word_length; i++) {			
 			if (guess [0] == whole_word[i]) {				
 				good_guess = 1;						
 				part_word [i] = whole_word[i];			
 			}
 		}

 		if (!good_guess) lives--;	

 		if (strcmp (whole_word, part_word) == 0) game_state = 'W';

 		else if (lives == 0)  {
 			game_state = 'L'; 						
 			strcpy (part_word, whole_word); 
 		}

 		sprintf (outbuf, "%s %d\n", part_word, lives);	
		sendto(out, outbuf, strlen(outbuf), 0, (struct sockaddr *) &cliAddr, slen);			// cast sockaddr_in to sockaddr
	}	
	
	// Send b to exit
	sprintf (outbuf, "%s\n", "bye");	
	sendto(out, outbuf, strlen(outbuf), 0, (struct sockaddr *) &cliAddr, slen);				// cast sockaddr_in to sockaddr
	close(in);
	shutdown(out, SHUT_RD);
}
