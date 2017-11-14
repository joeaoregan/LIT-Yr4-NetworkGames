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

int main(void) 
{
	int sock;
	int i;
	int byteCount;
	char buf[LINESIZE];

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
			

			if((byteCount = recvfrom(sock, buf, LINESIZE, 0, (struct sockaddr *) &si_other, &slen)) == -1)
			{
				displayErrMsg("recvfrom() Failed");
			}
	
			printf("Guess from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
			buf[byteCount-1] = '\0'; 
			printf("Received %d bytes: %s", byteCount, buf);
		

			if (sendto(sock, buf, strlen(buf), 0, (struct sockaddr *) &si_other, slen) == -1) 
			{
				displayErrMsg("sendto() Failed");
			}

			play_hangman(sock,sock);
			
	}

	close(sock);

	return 0;
}


/* ---------------- Play_hangman () ---------------------*/

void play_hangman (int in, int out) 
{
	printf("\nBegin Playing\n");

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
	
	send(out, outbuf, strlen(outbuf), 0);
		do
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
		}while(game_state == 'I');
 	
 }
