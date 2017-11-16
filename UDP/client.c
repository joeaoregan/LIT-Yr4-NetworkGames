// 20171116 CreateUDPSocket.h - abstracts the UDP socket creation code
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
#include "../CreateUDPSocket.h"

 int main (int argc, char * argv []) {					// Added arguments to specify another IP address other than the default localhost/127.0.0.1
	struct sockaddr_in si_other;
 	int s, i, byteCount;
	int slen=sizeof(si_other);
	char userInput[LINESIZE];
	char partword[LINESIZE];
// 	char format[LINESIZE];
	char* blah = "blah";						// output buffer

	s = createUDPClient((argc == 1) ? SRV_IP : argv[1]);
	si_other = getServerAddress((argc == 1) ? SRV_IP : argv[1]);

	printf("Playing Hangman\nPlease Enter Your Username: ");
	fgets(userInput, LINESIZE, stdin);				// Enter username
//	sprintf(format, "%s", userInput);				// copying a string to a string, not sure why???		

	if (sendto(s, userInput, strlen(userInput), 0,(struct sockaddr *) &si_other, slen) == -1) {
		displayErrMsg("sendto() Failed");
	}

// PLAY THE GAME
	while(partword[0] != 'b') {					// change to check lives, as this will exit on word beginning with b
// RECEIVE THE PART WORD
		partword[0]='\0';					// Reset the part-word string

		byteCount = recvfrom(s, partword, LINESIZE, 0, (struct sockaddr *) &si_other, &slen);	 	
		partword[byteCount-1] = '\0'; 				// bytecount -1 to remove \n or \r or whatever is giving new line		
		printf("Received:  %s", partword);			// Display the string received from the server	

// SEND GUESS
		fgets(userInput, LINESIZE, stdin);
//		sprintf(format, "%s", userInput);			// should be no need to do this?

		if (sendto(s, userInput, strlen(userInput), 0,(struct sockaddr *) &si_other, slen) == -1) {
			 displayErrMsg("sendto() Failed");
		}							
	}

	printf("Client Finished\n");

	close(s);
	return 0;
}
