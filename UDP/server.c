#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>			// exit()
#include "HandleErrors.h"		// Error handling functions header file

#define LINESIZE 80			// Length of the buffer
#define NPACK 10			// Number of packets to send
#define HANGMAN_TCP_PORT 1066		// Port number to connect on

 int maxlives = 12;
 char *word [] = {
 #include "words"			// Add the words from words file to an array
 };

int main(void) {
	struct sockaddr_in si_me, si_other;
	int sock, i, slen=sizeof(si_other);
	char buf[LINESIZE];

	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) displayErrMsg("socket");	// create UDP socket

	memset((char *) &si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(HANGMAN_TCP_PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	printf("Waiting for connections...\n");

	if (bind(sock, &si_me, sizeof(si_me)) == -1) displayErrMsg("bind");

	//for (i = 0; i < NPACK; i++) {
		if (recvfrom(sock, buf, LINESIZE, 0, &si_other, &slen)==-1) displayErrMsg("recvfrom()");
		//printf("Guess from %s:%d\nLetter: %c\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), buf[0]);
		printf("Guess from %s:%d\nReceived: %s\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), buf);
	//}

	close(sock);
	return 0;
}
