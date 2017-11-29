 /* 
 	File: 		UDPPlayHangman.h
	Version: 	play_hangman() function split to be used with UDP client/server
	Modified by:	Joe O'Regan

	Year 4 Networked Games Assignment

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690

	Similar to TCPPlayHangman.h only for UDP
*/

#ifndef	__PLAY_HANGMAN_UDP
#define	__PLAY_HANGMAN_UDP

#include <errno.h>

int checkGuessUDP(char* buf, char* word, char* part, char* guess, int lives);
void playHangmanUDP(int in, int out, char* name);

struct sockaddr_in cliAddr;									// IPv4 address structure
int slen = sizeof(cliAddr);
#define CLI_PORT ntohs(cliAddr.sin_port)							// Format the port number

/* --------------------------------------PLAY HANGMAN UDP --------------------------------------*/

/*
	SERVER UDP: 
	play_hangman() function, altered to work with UDP, using sendto() and recvfrom()
*/
void playHangmanUDP(int in, int out, char* name) {
 	char * whole_word, part_word [LINESIZE],guess[LINESIZE], outbuf[LINESIZE];		// Selected word, part word, input buffer, output buffer
 	int lives = MAX_LIVES, game_state = 'I', i, word_length;
 
	displayHostname();									// Socket.h: Display the local machines name
	
	whole_word = selectRandomWord(name, CLI_PORT);						// Hangman.h: Username and Port instead of IP and Port
 	word_length = strlen(whole_word);
 	
	initPartWord(part_word, word_length);							// Hangman.h: Set the part word to hyphens

	while (1) {		
/*SEND*/	sprintf (outbuf, "%s %d \n", part_word, lives);					// Format the part word and lives as a string to send to the client
		sendto(out, outbuf, LINESIZE, 0, (struct sockaddr *) &cliAddr, slen);		// cast sockaddr_in to sockaddr
		
		if ((game_state != 'I')) break;							// Check here, instead of while condition, as send/receive reversed from TCP version

/*RECV*/	if (recvfrom(in,guess,LINESIZE,0, (struct sockaddr *) &cliAddr, &slen) == -1) {	// cast sockaddr_in to sockaddr
			printf("Hangman recvfrom: No more input from %s%s%s\n",BLUE,name,NORM);	// Display end of input message, no need for error
			break;
		}

		//lives = checkGuessUDP(outbuf, whole_word, part_word, guess, lives, name, port);
		lives = checkGuessUDP(outbuf, whole_word, part_word, guess, lives);		// UDPPlayHangman.h: check client guess, and display formatted message on server side

		if ((game_state = checkGameState(whole_word, part_word, lives)) == 'L')		// Hangman.h: W = all letters guessed, L = if out of guesses/lives, otherwise return I
			strcpy(part_word, whole_word);						// Copy the selected word, to be sent to the client, to show Player

		printf("Game State: %c Lives: %d\n", game_state, lives);			// Display the game state and lives, for testing
	}	

/* GAME TERMINATES WITH 'b' FROM 'bye' */
	sprintf (outbuf, "%s\n", "bye");							// Client tests for letter 'b' as outbuf's 1st letter, as condition to exit it's loop
/*SEND*/sendto(out, outbuf, LINESIZE, 0, (struct sockaddr *) &cliAddr, slen);			// cast sockaddr_in to sockaddr

	close(in);										// Close the socket connection
}


/*
	SERVER UDP:
	Read in the username sent from the client, and display it on screen
	perform checks for no user input
*/
int getUserNameUDP(int s, char* name, struct sockaddr_in cliAddr) {
	int count;									// Count of bytes received
	int len = sizeof(cliAddr);							// Size of IPv4 client address stucture
	char input[LINESIZE];

	if((count = recvfrom(s,input,LINESIZE,0,(struct sockaddr*)&cliAddr,&len))==-1) {// Server receives 1st, recvfrom returns -1 if error
		printf("No more input from %s%s%s\n",BLUE,input,NORM);			// Display end of input message
		return 0;								// Return false, leaving the function without errors,
	} else {									// if no more input is received. Otherwise, 
		input[count-1] = '\0';							// terminate the end of the string (before the '\n' new line character)
		printf("Username received: %s%s%s\n",BLUE,input,NORM);			// Format and display the username
	}

	strcpy(name, input);

	printf("input: %s name: %s\n",input, name);

	return 1;									// Return a user name has been created is true
}


/*
	CLIENT:
	Send the guess input on the client side to the server
	Displaying an error if sendto() returns -1
*/
void sendGuess(int s, char* guess, struct sockaddr_in server) {
	int len = sizeof(server);							// size of the server address (should be passed in maybe)

	if (sendto(s, guess, strlen(guess), 0,(struct sockaddr *) &server, len) == -1)	// Send the guess entered on the client to the Server
		displayErrMsg("sendto() Failed");
}


/*
	SERVER UDP:
	Check input from client, format it, and display on server side, depending on the guess, 
	and decrement the number of lives if bad guess
	Returns the number of lives to be set in the client state.

//int checkGuessUDP(char* buf, char* word, char* part, char* guess, int lives, char* ip, int port) {
int checkGuessUDP(char* buf, char* word, char* part, char* guess, int lives) {
    if (!correctGuess(word, part, guess)) {
        lives--;										// Functionality moved to Hangman.h. Incorrect guess: decrement number of lives. Good guess: copy the letter to the part word
        //sprintf(buf, "%sBad Guess%s Received From Client %s/%d %s\n",RED,NORM,ip,port,guess);	// Format a bad guess received, adding the ip and port of the client to identify
        sprintf(buf, "%sBad Guess%s Received From Client %c\n",RED,NORM,guess[0]);		// Format a bad guess received, adding the ip and port of the client to identify
    }
    //else sprintf(buf,"%sGood Guess%s Received From Client %s/%d %s\n",GREEN,NORM,ip,port,guess);// Format a good guess received, adding the ip and port of the client to identify
    else sprintf(buf,"%sGood Guess%s Received From Client %c\n",GREEN,NORM,guess[0]);		// Format a good guess received, adding the ip and port of the client to identify
    write(0, buf, strlen(buf));									// Write the guess received to standard output, displaying on Server side

    return lives;										// Return the number of guesses/lives remaining
}
*/

#endif	/* __PLAY_HANGMAN_UDP */
