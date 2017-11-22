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


/*
	SERVER:
	Check input from client, format it, and display on server side, depending on the guess, 
	and decrement the number of lives if bad guess
	Returns the number of lives to be set in the client state.
*/
//int checkGuessUDP(char* buf, char* word, char* part, char* guess, int lives, char* ip, int port) {
int checkGuessUDP(char* buf, char* word, char* part, char* guess, int lives) {
    if (!correctGuess(word, part, guess)) {
        lives--;										// Functionality moved to Hangman.h. Incorrect guess: decrement number of lives. Good guess: copy the letter to the part word
        //sprintf(buf, "%sBad Guess%s Received From Client %s/%d %s\n",RED,NORM,ip,port,guess);	// Format a bad guess received, adding the ip and port of the client to identify
        sprintf(buf, "%sBad Guess%s Received From Client %c\n",RED,NORM,guess[0]);	// Format a bad guess received, adding the ip and port of the client to identify
    }
    //else sprintf(buf,"%sGood Guess%s Received From Client %s/%d %s\n",GREEN,NORM,ip,port,guess);// Format a good guess received, adding the ip and port of the client to identify
    else sprintf(buf,"%sGood Guess%s Received From Client %c\n",GREEN,NORM,guess[0]);// Format a good guess received, adding the ip and port of the client to identify
    write(0, buf, strlen(buf));									// Write the guess received to standard output, displaying on Server side

    return lives;										// Return the number of guesses/lives remaining
}

#endif	/* __PLAY_HANGMAN_UDP */
