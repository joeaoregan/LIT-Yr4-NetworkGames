// 20171115 Fixed client continuing to connect, 
// 		add command line parameter check
// 

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

 int main (int argc, char * argv []) {				// Added arguments to specify another IP address other than the default localhost/127.0.0.1
	struct sockaddr_in si_other;
 	int s, i, byteCount;
	int slen=sizeof(si_other);
	char guess[LINESIZE];
	char partword[LINESIZE];
 	char format[LINESIZE];
	char* blah = "blah";					// output buffer
 	char * server_name;	

	server_name = (argc == 1) ?  SRV_IP : argv[1];		// Can enter ./cli to run on localhost, or specify an address

	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)  displayErrMsg("Socket Failed");

	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(HANGMAN_TCP_PORT);

	if (inet_aton(server_name, &si_other.sin_addr) == 0 )  {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	printf("Playing Hangman\nPlease Enter Your Username: ");
	fgets(guess, LINESIZE, stdin);
	sprintf(format, "%s", guess);

	if (sendto(s, format, strlen(guess), 0,(struct sockaddr *) &si_other, slen) == -1) {
		displayErrMsg("sendto() Failed");
	}

// PLAY THE GAME
	while(partword[0] != 'b') {				// change to check lives, as this will exit on word beginning with b
// RECEIVE THE PART WORD
		partword[0]='\0';

		byteCount = recvfrom(s, partword, LINESIZE, 0, (struct sockaddr *) &si_other, &slen);	 	

		partword[byteCount-1] = '\0'; 			// bytecount -1 to remove \n or \r or whatever is giving new line		
		printf("Received:  %s", partword);	

// SEND GUESS
		fgets(guess, LINESIZE, stdin);
		sprintf(format, "%s", guess);

		if (sendto(s, format, strlen(guess), 0,(struct sockaddr *) &si_other, slen) == -1) {
			 displayErrMsg("sendto() Failed");
		}							
	}

	printf("Client Finished\n");

	close(s);
	return 0;
}
