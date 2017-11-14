#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "HandleErrors.h"
#include "../Hangman.h"
#include "../DrawHangman.h"

struct sockaddr_in si_me;
struct sockaddr_in si_other;

int slen = sizeof(si_other);

struct sessionData {				// Store the game state, maybe use a linked list of structs instead of array
	int sock;				// The client socket that is connected
	char* word;				// Store the current full word
	char partWord[LINESIZE];		// Store the current part word
	int lives;				// Store the number of guesses left for each client
	int gameState;				// Store the game state for the connection
	char bufIn[LINESIZE];			// BUFFER
	char bufOut[LINESIZE];			// BUFFER
	char ip[INET_ADDRSTRLEN];		// client IP
	int port;				// client port
};

int main(void) 
{
	struct 		sessionData client[FD_SETSIZE];	
	int sock;
	int i;
	int byteCount;

	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) 
	{
		displayErrMsg("Socket Failed");
	}

	printf("\nSocket Created\n");

	memset((char *) &si_me, 0, sizeof(si_me));

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(HANGMAN_TCP_PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sock, (struct sockaddr *) &si_me, sizeof(si_me)) == -1) 
	{	
		displayErrMsg("Bind Failed");
	}

	printf("\nSocket Binding Completed\n");

	drawHangman();

	while(1)
	{
		printf("\nWaiting For Data...\n");
		fflush(stdout);

		if(client[i].gameState == 'I')
		{
			

			if((byteCount = recvfrom(sock, buf, LINESIZE, 0, (struct sockaddr *) &si_other, &slen)) == -1)
			{
				displayErrMsg("recvfrom() Failed");
			}

			if((byteCount = recvfrom(sock, client[i].bufIn, LINESIZE)) == 0)
			{
				client[i].word = "";
				client[i].partWord[0] = '\0';
				client[i].lives = MAX_LIVES;
				client[i].gameState = 'I';
			}

			else
			{
				write(1, client[i]bufIn, n);

				if(!correctGuess(client[i].word, client[i].partWord, client[i].bugIn))
				{
					client[i].lives--;
				}

				if((client[i].gameState = checkGameState(client[i].
			}
	
			printf("Guess from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
			buf[byteCount-1] = '\0'; 
			printf("Received %d bytes: %s", byteCount, buf);
		

			if (sendto(sock, buf, strlen(buf), 0, (struct sockaddr *) &si_other, slen) == -1) 
			{
				displayErrMsg("sendto() Failed");
			}
		}
			
	}

	close(sock);

	return 0;
}



				write (1, client[i].bufIn, n);/*OK*/					// Display client guess on server side
/* GET FOR EACH CLIENT */
/* Decrement */					if (!correctGuess(client[i].word, client[i].partWord, client[i].bufIn)) 
							client[i].lives--;						// If it's an incorrect guess, decrement the number of lives

/* Show The Word */				if ((client[i].gameState = checkGameState(client[i].word, client[i].partWord, client[i].lives)) == 'L') 
							strcpy (client[i].partWord, client[i].word);			// Show the player the word if they lose

/* WRITE BACK TO CLIENT*/			sprintf(client[i].bufOut, "%s %d \n", client[i].partWord, client[i].lives);// Add the part-word and number of guesses left to the buffer string
						write(sockfd, client[i].bufOut, strlen(client[i].bufOut));		// Send the string to the client

/* WIN MSG ONLY */				//if (gameOverSelect(client[i].gameState,client[i].bufOut,client[i].word, sockfd, clntName, CLI_PORT)) close(sockfd);							// If game is finished, display win/lose message
/* WIN MSG ONLY */				gameOverSelect(client[i].gameState,client[i].bufOut,client[i].word, sockfd, clntName, CLI_PORT);
					}
				}	// CHANGED POSITION end when game state not = I
				if (--nready <= 0) break;

/* ---------------- Play_hangman () ---------------------*/

void play_hangman (int in, int out) 
{
	printf("Begin Playing\n");

 	char * whole_word, part_word [LINESIZE],
 	guess[LINESIZE], outbuf[LINESIZE];

 	int lives = MAX_LIVES;							
 	int game_state = 'I';							
 	int i, good_guess, word_length;
 	char hostname[LINESIZE];

 	gethostname (hostname, LINESIZE);
 	sprintf(outbuf, "Playing hangman on host %s: \n \n", hostname);			
 	write(out, outbuf, strlen (outbuf));						
 	
 	whole_word = word[rand() % NUM_OF_WORDS];
 	word_length = strlen(whole_word);
 	syslog (LOG_USER | LOG_INFO, "Server chose hangman word %s", whole_word);

	printf("Word chosen: %s\n", whole_word);

 	
 	for (i = 0; i < word_length; i++) 
	{
		part_word[i] = '-';
	}
 
	part_word[i] = '\0';

 	sprintf (outbuf, "%s %d \n", part_word, lives);
					
	sendto(out, outbuf, LINESIZE, 0, (struct sockaddr *) &si_other, slen);

 	while (game_state == 'I') 
	{ 							
 		if (recvfrom(in, guess, LINESIZE, 0, &si_other, &slen) == -1)
		{
			 displayErrMsg("recvfrom() Failed");
		}

	 	good_guess = 0;

	 	for (i = 0; i < word_length; i++) 
		{			
	 		if (guess [0] == whole_word[i]) 
			{				
	 			good_guess = 1;						
	 			part_word [i] = whole_word[i];			
	 		}
	 	}

	 	if (! good_guess) 
		{	
			lives--;	
		}				

	 	if (strcmp (whole_word, part_word) == 0) 
		{
			game_state = 'W';
		}

	 	else if (lives == 0) 
		{
	 		game_state = 'L'; 						
	 		strcpy (part_word, whole_word); 
	 	}

	 	sprintf (outbuf, "%s %d \n", part_word, lives);	
		sendto(out, outbuf, LINESIZE, 0, &si_other, slen);
 	}
 }
