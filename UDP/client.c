#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "../HandleErrors.h"
#include "../Hangman.h"

int main(void) {
	struct sockaddr_in si_other;
 	int s, i, byteCount;
	int slen=sizeof(si_other);
	char guess[LINESIZE];
	char partword[LINESIZE];
 	char format[LINESIZE];
	char* blah = "blah";

	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)  displayErrMsg("Socket Failed");

	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(HANGMAN_TCP_PORT);

	if (inet_aton(SRV_IP, &si_other.sin_addr) == 0 )  {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	while(1) {
		fgets(guess, LINESIZE, stdin);

		//printf("blah strlen: %lu, sizeof: %lu\n", strlen(blah), sizeof(blah));
		//printf("Keyboard input strlen: %lu, sizeof: %lu\n", strlen(guess), sizeof(guess));

		sprintf(format, "%s", guess);

		if (sendto(s, format, strlen(guess), 0,(struct sockaddr *) &si_other, slen) == -1) {
			 displayErrMsg("sendto() Failed");
		}

		partword[0]='\0';
		printf("partword: %s\n", partword);

		byteCount = recvfrom(s, partword, LINESIZE, 0, (struct sockaddr *) &si_other, &slen);	 	

		partword[byteCount] = '\0'; 									
		printf("Received:  %s, strlen: %lu, bytes (including strings null-terminator): %d\n", partword, strlen(partword), byteCount);										
	}

	close(s);
	return 0;
}
