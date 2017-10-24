#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>			// Warning for exit()
#include "HandleErrors.h"		// Error handling functions header file

#define LINESIZE 80			// Buffer length
#define NPACK 10			// Number of packets to send
#define HANGMAN_TCP_PORT 1066		// Server Port Number
#define SRV_IP "127.0.0.1"		// IP Address of server

int main(void) {
	struct sockaddr_in si_other;
 	int s, i, slen=sizeof(si_other);
	char buf[LINESIZE];
 	char o_line[LINESIZE];

	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) displayErrMsg("socket");

	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(HANGMAN_TCP_PORT);

	if (inet_aton(SRV_IP, &si_other.sin_addr) ==0 ) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	read (0, o_line, LINESIZE);			// 0 = STDIN
	//printf("Sending letter: %s\n", o_line);		
	sprintf(buf, "%s", o_line);
	if (sendto(s, buf, LINESIZE, 0, &si_other, slen) == -1) displayErrMsg("sendto()");

	close(s);
	return 0;
}
