/* 
	client.c

	UDP client socket
	
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
#include <stdio.h>			// gets()?
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>			// Warning for exit()
#include "HandleErrors.h"		// Error handling functions header file
#include "../Hangman.h"			// Include Hangman game header file from parent directory

#define SRV_IP "127.0.0.1"		// IP Address of server

int main(void) {
	struct sockaddr_in si_other;
 	int s, i, slen=sizeof(si_other), byteCount;
	char guess[LINESIZE];
	char partword[LINESIZE];
 	char format[LINESIZE];

	char* blah = "blah";

	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) displayErrMsg("socket");

	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(HANGMAN_TCP_PORT);

	if (inet_aton(SRV_IP, &si_other.sin_addr) ==0 ) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}
/*
	printf("OK 1\n");

// Get initial part word from server
	if (recvfrom(s, guess, LINESIZE, 0, &si_other, &slen) == -1) displayErrMsg("recvfrom()");
	printf("Guess from %s:%d\nLetter: %c\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), guess[0]);


	printf("OK 2\n");
*/
// Input character from keyboard

	//read (0, guess, LINESIZE);			// Get input from STDIN (0 = STDIN)
	//gets(guess);
	fgets(guess, LINESIZE, stdin);

	printf("blah strlen: %lu, sizeof: %lu\n", strlen(blah), sizeof(blah));
	printf("Keyboard input strlen: %lu, sizeof: %lu\n", strlen(guess), sizeof(guess));

	sprintf(format, "%s", guess);
// SEND
	if (sendto(s, format, strlen(guess), 0,(struct sockaddr *) &si_other, slen) == -1) displayErrMsg("sendto()");	// need to cast sockaddr_in to sockaddr

// RECV
	partword[0]='\0';
	printf("partword: %s\n", partword);

	byteCount = recvfrom(s, partword, LINESIZE, 0, (struct sockaddr *) &si_other, &slen);				// need to cast sockaddr_in to sockaddr
	//printf("Message from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
	partword[byteCount] = '\0'; 											// Need to null-terminate received data (TCP/IP Sockets in C chapter 4: P60/55)
	printf("Received:  %s, strlen: %lu, bytes (including strings null-terminator): %d\n", partword, strlen(partword), byteCount);


	close(s);
	return 0;
}
