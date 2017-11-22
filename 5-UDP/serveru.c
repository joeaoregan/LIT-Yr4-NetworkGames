 /* 
 	File: 		serveru.c
	Version: 	Hangman UDP Server
	Modified by:	Samantha Marah
			Joe O'Regan

	Year 4 Networked Games Assignment

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690

	20171116 	CreateUDPSocket.h - abstracts the UDP socket creation code
	20171115 	Fixed client continuing to connect, 
			add command line parameter check
			Fixed warning messages by casting sockaddr_in to struct sockaddr*
			And moved HandleErrors to parent folder	
			Added random word selection	
*/

#include <unistd.h>												// write(), gethostname()
#include <string.h>												// strlen()
#include <stdlib.h>												// exit(), rand(), srand()
#include <time.h>												// time(), Seed the random number
#include "../HandleErrors.h"											// Moved HandleErrors.h to parent folder
#include "../Hangman.h"
#include "../DrawHangman.h"
#include "../CreateUDPSocket.h"

//struct sockaddr_in srvAddr;
struct sockaddr_in cliAddr;

int slen = sizeof(cliAddr);

int main(void) {
	int sock, i, byteCount;
	char buf[LINESIZE];

	sock = createUDPServer();

	drawHangman();

	printf("\nWaiting For Data...\n");

	while(1) {
		fflush(stdout);
		srand(time(NULL));
// RECEIVE USERNAME
		if((byteCount = recvfrom(sock, buf, LINESIZE, 0, (struct sockaddr *) &cliAddr, &slen)) == -1) {	// Server receives 1st, recvfrom returns -1 if error
			displayErrMsg("recvfrom() Failed");
		}
		
		buf[byteCount-1] = '\0';

		printf("Username received: %s\n",buf);

		play_hangman(sock,sock);
		printf("Finished Play Hangman\n");		
	}

	close(sock);	

	return 0;
}

/* ---------------- Play_hangman () UDP ---------------------*/
void play_hangman (int in, int out) {
 	char * whole_word, part_word [LINESIZE],guess[LINESIZE], outbuf[LINESIZE];
 	int lives = MAX_LIVES, game_state = 'I', i, good_guess, word_length;
 	char hostname[LINESIZE];

 	gethostname (hostname, LINESIZE);
 	sprintf(outbuf, "Playing hangman on host %s: \n \n", hostname);			
 	write(0, outbuf, strlen (outbuf));						
 	
	whole_word = selectRandomWord("Client", 0000);							// FIX THIS LATER - CLIENT IP AND PORT
 	word_length = strlen(whole_word);
 	
 	for (i = 0; i < word_length; i++) {
		part_word[i] = '-';
	} 
	part_word[i] = '\0';

 	sprintf (outbuf, "%s %d \n", part_word, lives);	
	sendto(out, outbuf, strlen(outbuf), 0, (struct sockaddr*) &cliAddr, sizeof cliAddr);	

	while (game_state == 'I') {						
		if (recvfrom(in, guess, LINESIZE, 0, (struct sockaddr *) &cliAddr, &slen) == -1){	// cast sockaddr_in to sockaddr
			 displayErrMsg("recvfrom() Failed");
		}

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

 		sprintf (outbuf, "%s %d \n", part_word, lives);	
		sendto(out, outbuf, LINESIZE, 0, (struct sockaddr *) &cliAddr, slen);			// cast sockaddr_in to sockaddr

		printf("Game State %c\n", game_state);
	}	

	sprintf (outbuf, "%s\n", "bye");	
	sendto(out, outbuf, LINESIZE, 0, (struct sockaddr *) &cliAddr, slen);			// cast sockaddr_in to sockaddr
	//close(in);
}
