 /* 
 	File: 		Hangman.h
	Version: 	Hangman game functionality, mostly Server side
	Author:		Joe O'Regan

	Year 4 Networked Games Assignment

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690

	22/11/2017	parseWordAndLives() moved here from DrawHangman.h	
			Modified, fork, and select servers now all use checkGameOver()
*/

#ifndef	__HANGMAN_H
#define	__HANGMAN_H

#include <syslog.h>

char* word [] = {										// Array of words, one word will be selected randomly to guess
  #include "words"										// words text file
};

#define NUM_OF_WORDS (sizeof (word) / sizeof (word [0]))					// The number of words in the word array, used to randomly select a word
#define MAX_LIVES 12										// The number of guesses the player can make
#define LINESIZE 80										// Maximum size in the world of Any string


/*
	SERVER:
	Random word selection with IP/Port Info: 
	Pick a word at random from the word list, displaying the Client IP & Port
*/
char* selectRandomWord(char* ip, int port) {  
  char* the_word = word[rand() % NUM_OF_WORDS];							// Select random word from words array for client to guess

  syslog (LOG_USER | LOG_INFO, "server chose hangman word %s", the_word);			// Message logging

  printf("Word: %s\"%s\"%s Randomly Selected For Client: %s/%d\n",RED,the_word,NORM,ip,port);	// Display the word selected for the client on the server, highlighting the word in red font

  return the_word;										// Return the selected word
}


/*
	SERVER:
	Basic random word selection: 
	Pick a word at random from the word list
*/
char* randomWord() {  
  char* the_word = word[rand() % NUM_OF_WORDS];							// Select random word from words array for client to guess

  printf("Word: \"%s\" Randomly Selected\n", the_word);						// Display the word selected for the client on the server

  return the_word;										// Return the selected word
}


/*
	SERVER:
	Set the part word to hyphens
*/
char initPartWord(char* partWord, int length) {
	int i;											// For loop index
 	for (i = 0; i < length; i++) partWord[i]='-';						// Set every character in the partword to a hyphern
 	
	partWord[i] = '\0';									// Set the string terminator
}


/*
	CLIENT:
	Parse the string from the server with the part word and number of lives
	If the draw variable is set, call the DrawHangman.h amputate() to
	render the graphical version of the number of lives left
	Display the remaining guesses and the part word while the game is playing
	or the full word if the game is finished and the player didn't guess
*/
int parseWordAndLives(char* input, int draw) {
	char word[20];
	int lives;

	sscanf(input, "%s %d", &(*word), &lives);						// Parse string data received from server into separate part-word and score variables

	if (draw) amputate(lives);								// If the draw paramater has been set, display the graphics for lives

	printf("\n%sRemaining Guesses:%s\t%d\n", CYAN, NORM, lives);				// Guesses remaining
	if (lives > 0) printf("%sWord To Guess:%s\t\t%s\n", CYAN, NORM, word);			// The part word string while game is playing OR
	else printf("%sThe word was:%s\t\t%s\n", RED, NORM, word);				// The actual word if player loses

	return lives;										// Return the number of lives, to decide which game over message
}


/*
	SERVER TCP & UDP: 
	Check if a correct guess has been made or not
*/
int correctGuess(char* word, char* part, char* guess) {
  //printf("part word before: %s\n", part);
  int i, good_guess = 0;									// Set/reset good_guess to false
    for (i = 0; i < strlen(word); i++) {
      if (guess [0] == word [i]) {								// If the guess from client is a match
        good_guess = 1;										// Set good guess true
        part[i] = word[i];									// Fill the guessed letter in correct position
      }
    }
  //printf("part word after: %s\n", part);

  return good_guess;										// Return 1 if good guess, and 0 if the letter isn't in the whole word
}


/*
	Check the game state, and decide if win/lose/continue
*/
char checkGameState(char* word, char* part, int lives) {
	if (strcmp (word, part) == 0) return 'W'; 						// If all letters guessed correctly. Game state becomes W ==> User Won
	else if (lives <= 0) return 'L';							// If client has run out of guesses. Game state becomes L ==> User Lost

	return 'I';	
}


/*
	SERVER:
	Display end of game message if game complete, with Client IP and Address
	Return 1 if game is finished
*/
int checkGameOver(int state, char* outbuffer, char* word, int o, char* clname, int clport) {
  if (state == 'I') return 0;									// return false, skip the rest of this function

  if (state == 'W') printf("Client %s/%d has guessed \"%s\" correctly!\n", clname,clport,word); // Display win message
    else if (state == 'L') printf("Client %s/%d is a loser!\n", clname, clport);		// Display lose message

  return 1;											// Return true
}


/*
	SERVER:
	Display end of game message if game complete

int gameOverSelect(int state, char* outbuffer, char* word, int o, char* name, int port) {
  //char buf[LINESIZE] = "";
  //char msg[LINESIZE] = "";
  //msg[0] = '\0';
  int count;

  if (state == 'I') {
    return 0;											// return false, skip the rest of this function
  }
  else if (state == 'W') {
    snprintf(outbuffer, LINESIZE, "Client %s/%d has guessed \"%s\" correctly!\n", name, port, word);// Format the Server side win message
    write(0, outbuffer, strlen(outbuffer));							// Display win message on Server
    //printf("Client %s/%d has guessed \"%s\" correctly!\n", clname, clport, word);		// Display win message
    snprintf (outbuffer, LINESIZE, "Player Has Guessed \"%s\" Correctly!\n", word);		// Set win message to return to client
    write(o, outbuffer, strlen(outbuffer));	
  }
  else if (state == 'L') {
    count = snprintf(outbuffer, LINESIZE, "Client %s/%d is a loser!\n", name, port);		// Format the Server side lose message
    write(0, outbuffer, count);									// Display lose message on Server
    count = snprintf(outbuffer, LINESIZE, "Player Has Run Out Of Guesses!\n");			// Set lose message to return to client, with protection against buffer overflow
    write (o, outbuffer, count);								// Send outbuf info to client
  }

    return 1;											// return true
}
*/

#endif	/* __HANGMAN_H */
