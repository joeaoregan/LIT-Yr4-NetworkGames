 /* 
	Client for hangman server

 	File: Hangclient.c

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690

	19/09/2017 	Added includes to get rid of compile warnings (wasn't necessary)
			Game Over message displayed after connection to Server terminates
	23/09/2017	Added include to clear warning on read(), write()
			Added hangman graphic
*/

 #include <stdio.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <netdb.h>
 #include <string.h>	// 19/09/2017 Warning for memcpy
 #include <stdlib.h>	// 19/09/2017 Warning for exit
 #include <unistd.h>	// 24/09/2017 write(), read()

 # define LINESIZE 80
 # define HANGMAN_TCP_PORT 1066

char *hangman[]= {
"   T E A M 2",
" H A N G M A N",
"  _____________", 
"  |/      |",
"  |   ___(\")___",
"  |      |_|",
"  |      /^\\",
"  |    _// \\\\_",
"__|____________"};    

char *altArms[]={"  |    __(\")___", "  |    __(\")__", "  |      (\")___", "  |      (\")"};	// ARMS - left hand, right hand, left arm, none
char *altBody[]={"  |"};									// BODY - none
char *altLegs[]={"  |      ~^\\", "  |      ~^~",};						// LEGS - 0.Left, 1.right, 2.none
char *altFeet[]={"  |     // \\\\_","  |     // \\\\", "  |        \\\\_", "  |"};		// Feet - 0.Left foot, 1.right foot, 2.Left shin, 3. Right shin (none)

 int main (int argc, char * argv [])
 {
 	struct sockaddr_in server; /* Server's address assembled here */
 	struct hostent * host_info;
 	int sock, count;
 	char i_line[LINESIZE];
 	char o_line[LINESIZE];
 	char * server_name;

 	/* Get server name from the command line.  If none, use 'localhost' */

 	server_name = (argc = 1)?  argv [1]: "localhost";

 	/* Create the socket */
 	sock = socket (AF_INET, SOCK_STREAM, 0);
 	if (sock <0) {
 	  perror ("Creating stream socket");
 	  exit (1);
 	}

 	host_info = gethostbyname(server_name);
 	if (host_info == NULL) {
 	  fprintf (stderr, "%s: unknown host:%s \n", argv [0], server_name);
 	  exit (2);
 	}

 	/* Set up the server's socket address, then connect */

 	server.sin_family = host_info->h_addrtype;
 	memcpy ((char *) & server.sin_addr, host_info->h_addr, host_info->h_length);
 	server.sin_port = htons (HANGMAN_TCP_PORT);

 	if (connect (sock, (struct sockaddr *) & server, sizeof server) <0) {
 	  perror ("connecting to server");
 	  exit (3);
 	}

	// Draw hangman
	int h;
	for (h = 0; h < 9; ++h){
	  printf("%s\n", hangman[h]);
	}

 	/*OK connected to the server.  
 	Take a line from the server and show it, take a line and send the user input to the server. 
 	Repeat until the server terminates the connection. */

 	printf ("Connected to server: %s \n", server_name);					// 19/09/2017 Change formatting

	//int i, numLetters = 0;

 	while ((count = read (sock, i_line, LINESIZE)) > 0) {
 	    write (1, i_line, count);	

	    // parse server stuff
	    //for (i = 0; i < LINESIZE; i++) {
	//	if (i_line[i] == '-') numLetters++;
	//	else if (i_line[i] == ' ') break;
	  //  }

 	    // write (1, i_line, count);							// Display line from server on screen
 	    count = read (0, o_line, LINESIZE);							// 0 = STDIN
 	    write (sock, o_line, count);							// Send client input to server


	    //printf("Num letters: %d\n", numLetters);
	    // numLetters = 0;
	    //sprintf (i_line, "\nNum letters:  %d\n\n\r", 16);
	   // write (0, i_line, 16); 
	    //numLetters = 0;
 	}

	printf("Game Over!\n");									// 19/09/2017 Add simple game over message to test prog terminates
 }
