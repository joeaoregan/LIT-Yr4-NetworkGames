 /* Network server for hangman game */
 /* File: hangserver.c */

 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <stdio.h>
 #include <syslog.h>
 #include <signal.h>
 #include <errno.h>
 #include <string.h>							// 23/09/2017 Warning for strcpy
 #include <stdlib.h>							// 23/09/2017 Warning for exit
 #include <unistd.h>							// 23/09/2017 gethostname(), write(), read(), close()
 #include <arpa/inet.h>							// 23/09/2017 inet_ntop()

 extern time_t time ();

 int maxlives = 12;
 char *word [] = {
 # include "words"
 };
 # define NUM_OF_WORDS (sizeof (word) / sizeof (word [0]))
 # define MAXLEN 80 							/* Maximum size in the world of Any string */
 # define HANGMAN_TCP_PORT 1066					// The port number

 char clntName[INET_ADDRSTRLEN];				// Client address string
 struct sockaddr_in server, client;

 void play_hangman(int in, int out);			// Function delcaration for play_hangman

 void main ()									// No command line arguments
 {
 	int sock, fd, client_len;
 	//struct sockaddr_in server, client;		// Made global to display client name outside main

 	srand ((int) time ((long *) 0)); 			/* randomize the seed */

 	sock = socket (AF_INET, SOCK_STREAM, 0);	// 0 or IPPROTO_TCP	// Create the socket
 	if (sock <0) { 								// This error checking is the code Stevens wraps in his Socket Function etc
 		perror ("creating stream socket");
 		exit (1);
 	}

 	server.sin_family = AF_INET;				// IPv4 address
 	server.sin_addr.s_addr = htonl(INADDR_ANY);	// Server IP
 	server.sin_port = htons(HANGMAN_TCP_PORT);	// Server port

 	if (bind(sock, (struct sockaddr *) & server, sizeof(server)) <0) {
 		perror ("binding socket");
	 	exit (2);
 	}

 	listen (sock, 5);

 	while (1) {
 		client_len = sizeof (client);
 		if ((fd = accept (sock, (struct sockaddr *) &client, &client_len)) <0) {
 			perror ("accepting connection");
 			exit (3);
 		}

		// Display Client address and port
		if (inet_ntop(AF_INET, &client.sin_addr.s_addr, clntName,sizeof(clntName)) != NULL){
  			printf("Handling client %s/%d\n", clntName, ntohs(client.sin_port));
		}

 		play_hangman (fd, fd);					// Play the game
 		close (fd);								// Close the connection to client
 	}
 }

 /* ---------------- Play_hangman () ---------------------*/

 void play_hangman (int in, int out)
 {
 	char * whole_word, part_word [MAXLEN],
 	guess[MAXLEN], outbuf [MAXLEN];

 	int lives = maxlives;
 	int game_state = 'I';											// I = Incomplete
 	int i, good_guess, word_length;									// Iterator, correct guesses, 
 	char hostname[MAXLEN];

 	gethostname (hostname, MAXLEN);
 	sprintf(outbuf, "Playing hangman on host: %s \n \n", hostname);
 	write(out, outbuf, strlen (outbuf));

 	/* Pick a word at random from the list */
 	whole_word = word[rand() % NUM_OF_WORDS];						// Select random word from words array
 	word_length = strlen(whole_word);								// Get length of word
 	syslog (LOG_USER | LOG_INFO, "server chose hangman word %s", whole_word);

 	/* No letters are guessed Initially */
 	for (i = 0; i <word_length; i++)
 		part_word[i]='-';											// Display hyphens to indicate size of word
 	
	part_word[i] = '\0';											// Add string terminating character

 	sprintf (outbuf, "%s %d \n", part_word, lives);					// Set outbuf to the part word & the lives left
 	write (out, outbuf, strlen(outbuf));							// Send to client

 	while (game_state == 'I')										// Loop until game is complete
 	/* Get a letter from player guess */
 	{
		while (read (in, guess, MAXLEN) <0) {						// Read guess from client
 			if (errno != EINTR)										// Check for error
 				exit (4);
 			printf ("re-read the startin \n");
 			} 														/* Re-start read () if interrupted by signal */
 		good_guess = 0;												// Set/reset good_guess to false
 		for (i = 0; i <word_length; i++) {
 			if (guess [0] == whole_word [i]) {						// If the guess from client is a match
 				good_guess = 1;										// Set good guess true
 				part_word [i] = whole_word [i];						// Fill the guessed letter in correct position
 			}
 		}
 		if (! good_guess) lives--;									// If the player guesses wrong, decrement lives
 		if (strcmp (whole_word, part_word) == 0)					// If all letters guessed correctly
 			game_state = 'W'; 										/* W ==> User Won */
 		else if (lives == 0) {										// If client has run out of guesses
 			game_state = 'L'; 										/* L ==> User Lost */
 			strcpy (part_word, whole_word); 						/* User Show the word */
 		}
 		sprintf (outbuf, "%s %d \n", part_word, lives);				// Set the part_word and lives to be sent
 		write (out, outbuf, strlen (outbuf));						// Send outbuf info to client
	
		// Send game over message
		if (game_state == 'W') {
			printf("Client %s/%d has guessed correctly!\n", clntName, ntohs(client.sin_port));
 			sprintf (outbuf, "\nPlayer Has Guessed Correctly!\n");	// Set win message to return to client
 			write (out, outbuf, strlen (outbuf));					// Send outbuf info to client
		}
		else if (game_state == 'L') {
			printf("Client %s/%d is a loser!\n", clntName, ntohs(client.sin_port));
 			sprintf (outbuf, "\nPlayer Has Run Out Of Guesses!\n");	// Set lose message to return to client
 			write (out, outbuf, strlen (outbuf));					// Send outbuf info to client
		}
 	}
 }
