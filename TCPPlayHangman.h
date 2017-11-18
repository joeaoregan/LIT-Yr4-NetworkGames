 /* 
 	File: 		TCPPlayHangman.h
	Version: 	play_hangmang() function
	Author:		Joe O'Regan

	Year 4 Networked Games Assignment

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690

	Reconfigured version of SetupTCPServerSocket() function from TCPServerUtility.c
	from TCP/IP Sockets in C book, to work with TCP Hangman fork() server

	18/11/2017 	Moved play_hangman() functionality to its own file to be used by
			both the modified and fork() versions of the game
*/

#ifndef	__PLAY_HANGMAN_TCP
#define	__PLAY_HANGMAN_TCP

#include <errno.h>										// EINTR

/* ---------------- Play_hangman () ---------------------*/

//void playHangmanTCP(int in, int out) {
void playHangmanTCP(int in, int out, char* name, int port) {
	char * whole_word, part_word [LINESIZE],						// LINESIZE is declared in Hangman.c replacing MAXLEN
	guess[LINESIZE], outbuf [LINESIZE];
 	int i, word_length, lives = MAX_LIVES, game_state = 'I', byteCount;			// Iterator, length of whole_word, correct guesses, game state
 	
  	displayHostname(out, outbuf);								// Function in Hangman.h: Display the name of the server

	/* RANDOM WORD SELECTION FUNCTION */
 	whole_word = whole_word = selectRandomWord(name, port);					// XXX Function in Hangman.h: Select random word from words array
 	//whole_word = whole_word = randomWord();						// Function in Hangman.h: Select random word from words array
 	word_length = strlen(whole_word);							// Get length of word

 	/* No letters are guessed Initially */
 	for (i = 0; i <word_length; i++)
 		part_word[i]='-';								// Display hyphens in place of letters, to indicate size of word
 	
	part_word[i] = '\0';									// Add string terminating character

 	sprintf (outbuf, "%s %d \n", part_word, lives);						// Set outbuf to the part word & the lives left, creating a string to send to the client
 	write(out, outbuf, strlen(outbuf));							// Send part word string to client

 	while (game_state == 'I') {								// Loop until game is complete /* Get a letter from player guess */
		while ((byteCount = read(in, guess, LINESIZE)) < 0) {				// Read guess from client
 			if (errno != EINTR) displayErrMsgStatus("Re-read the startin\n", 4);	// P182 Check for EINTR error if stuck in slow system call, & exit if restart signal not recieved. Re-start read() if interrupted by signal
 		} 
//		guess[byteCount-1] = '\0';
		sprintf(outbuf, "Guess received from Client %s/%d %s", name,port,guess);	// Display the guess received, and the ip and port of the client
		write(0, outbuf, strlen(outbuf));

		/* CHECK GUESS FUNCTION */
   		if (!correctGuess(whole_word, part_word, guess)) lives--;			// Functionality moved to Hangman.h. Incorrect guess: decrement number of lives. Good guess: copy the letter to the part word

 		if (strcmp (whole_word, part_word) == 0)					// If all letters guessed correctly
 			game_state = 'W'; 							// W ==> User Won 
 		else if (lives == 0) {								// If client has run out of guesses
 			game_state = 'L'; 							// L ==> User Lost
 			strcpy (part_word, whole_word); 					// User Show the word
 		}

 		sprintf (outbuf, "%s %d \n", part_word, lives);					// Set the part_word and lives to be sent
 		write(out, outbuf, strlen(outbuf));						// Send outbuf info to client

		/* DISPLAY WIN/LOSE MESSAGE */	
  		//checkGameOver2(game_state, outbuf, whole_word, out); 				// XXX Hangman.h function: If game is finished, display win/lose message
  		checkGameOver(game_state, outbuf, whole_word, out, name, port);			// Hangman.h function: If game is finished, display win/lose message
 	}
 }

#endif	/* __PLAY_HANGMAN_TCP */
