 /* 
 	File: 		TCPPlayHangman.h
	Version: 	play_hangman() function split to be used with modified, fork, and select hangman servers
	Modified by:	Joe O'Regan

	Year 4 Networked Games Assignment

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690

	Reconfigured version of SetupTCPServerSocket() function from TCPServerUtility.c
	from TCP/IP Sockets in C book, to work with TCP Hangman fork() server

	22/11/2017	checkGuess() now highlights in colour if it was a good/bad guess in
			text displayed on Server
			sendPartWord() is used to send the part word/lives string to the client
	18/11/2017 	Moved play_hangman() functionality to its own file to be used by
			both the modified and fork() versions of the game
*/

#ifndef	__PLAY_HANGMAN_TCP
#define	__PLAY_HANGMAN_TCP

#include <errno.h>										// EINTR, Defines errno int, set by system calls/library functions, to indicate error
#include <ctype.h>										// tolower(): take uppercase and lowercase input from client

void sendPartWord(int out, char* outbuf, char* part_word, int lives);				// Function to format and send the part word and lives data to client as a string


/*--------------------------------------- PLAY HANGMAN -----------------------------------------*/

/*
	SERVER TCP:
	play_hangman() function, altered from original, working with modified, and fork TCP sockets
*/
void playHangmanTCP(int in, int out, char* name, int port) {
	char * whole_word, part_word [LINESIZE],					// LINESIZE is declared in Hangman.c replacing MAXLEN
	guess[LINESIZE], outbuf [LINESIZE];						// Guess input buffer from client, outward buffer to client
 	int i, word_length, lives = MAX_LIVES, game_state = 'I', byteCount;		// Iterator, length of whole_word, correct guesses, game state
 	
  	displayHostname(out, outbuf);							// Socket.h: Display the name of the server on the client

	/*------------------------- RANDOM WORD SELECTION FUNCTION ---------------------*/
 	whole_word = selectRandomWord(name, port);					// Hangman.h: Select random word from words array
 	word_length = strlen(whole_word);						// Get length of word

 	/*------- INITIALISE AND SEND PART WORD: No letters are guessed Initially ------*/
	initPartWord(part_word, word_length);						// Hangman.h: Display hyphens in place of letters, indicate word size	
	sendPartWord(out, outbuf, part_word, lives);					// Format and send the part word and number of lives to the Client

	/*----------------------------- COMMUNICATION LOOP  ----------------------------*/
 	while (game_state == 'I') {							// Loop until game is complete /* Get a letter from player guess */

		/*----------------- GET CLIENT INPUT: read in the guess ----------------*/
		while ((byteCount = read(in, guess, LINESIZE)) < 0) {			// Read guess from client
 			if(errno!=EINTR) displayErrMsgStatus("Re-read the startin\n",4);// Check for EINTR, error if stuck in slow system call, & exit if
 		} 									// restart signal not recieved. Re-start read() if interrupted by signal
		guess[1]='\0';								// Only need 1 letter, so this will cut all the gibberish
		tolower(guess[0]);							// Set input to lower case, to accept Capital letters

		/*-------------CHECK CORRECT GUESS & FORMAT INFO------------------------*/
 
		checkGuess(outbuf, whole_word, part_word, guess, &lives, name, port);	// Decrement the lives remaining if the client sends an incorrect guess

		/*-----------------CHECK GAME WIN LOSE CONDITIONS-----------------------*/
		if((game_state = checkGameState(whole_word,part_word,lives)) == 'L')	// Hangman.h: check if the game state is win/lose/continue, and if lose
			strcpy(part_word, whole_word);					// copy the full word to part word, to show Player the word

		/*---------------------- SEND NEW PART WORD/SCORE ----------------------*/
		sendPartWord(out, outbuf, part_word, lives);				// Format and send the part word and number of lives to the Client

		/*---------------------- DISPLAY WIN/LOSE MESSAGE ----------------------*/	
  		checkGameOver(game_state, outbuf, whole_word, out, name, port);		// Hangman.h: If game is finished, display win/lose message
 	}
 }
	

/*	
	SERVER TCP:
	Function to format and send the part word and lives data to client as a string
	from the server using write()
*/
void sendPartWord(int out, char* outbuf, char* part_word, int lives) {
	int byteCount;							// Number of bytes that will be sent to the client

 	sprintf(outbuf, "%s %d \n", part_word, lives);			// Set outbuf to part word & lives left, creating string to send to the client
 	byteCount = write(out, outbuf, strlen(outbuf));			// Send part word string to client
	printf("Number of bytes sent: %d in: %s", byteCount, outbuf);	// Display number of bytes sent for part word, lives, new line & string terminator
}

#endif	/* __PLAY_HANGMAN_TCP */
