#ifndef	__HANGMAN_H
#define	__HANGMAN_H

#include <syslog.h>

char* word [] = {										// Array of words, one word will be selected randomly to guess
  #include "words"										// words text file
};

//#define MAX_WORD_LENGTH 11									// The longest word has 11 characters
#define NUM_OF_WORDS (sizeof (word) / sizeof (word [0]))					// The number of words in the word array, used to randomly select a word
#define MAX_LIVES 12										// The number of guesses the player can make
#define LINESIZE 80										// Maximum size in the world of Any string
//#define	MAX_CLIENTS 5									// 2nd argument to listen(), the maximum number of client connections
//#define	MAXLINE	4096									// max text line length

void play_hangman(int in, int out);								// Function delcaration for play_hangman
char checkGameState(char* word, char* part, int lives);

// Display end of game message if game complete, with Client IP and Address
void checkGameOver(int state, char* outbuffer, char* word, int o, char* clname, int clport) {
  if (state == 'W') {
    printf("Client %s/%d has guessed \"%s\" correctly!\n", clname, clport, word);		// Display win message
    snprintf (outbuffer, LINESIZE, "Player Has Guessed \"%s\" Correctly!\n", word);		// Set win message to return to client
    write (o, outbuffer, strlen (outbuffer));							// Send outbuf info to client
  }
  else if (state == 'L') {
    printf("Client %s/%d is a loser!\n", clname, clport);					// Display lose message
    snprintf (outbuffer, LINESIZE, "Player Has Run Out Of Guesses!\n");				// Set lose message to return to client, with protection against buffer overflow
    write (o, outbuffer, strlen (outbuffer));							// Send outbuf info to client
  }
}

// Display end of game message if game complete
void checkGameOver2(int state, char* outbuffer, char* word, int o) {
  if (state == 'W') {
    printf("Client has guessed \"%s\" correctly!\n",  word);		// Display win message
    snprintf (outbuffer, LINESIZE, "Player Has Guessed \"%s\" Correctly!\n", word);		// Set win message to return to client
    write (o, outbuffer, strlen (outbuffer));							// Send outbuf info to client
  }
  else if (state == 'L') {
    printf("Client is a loser!\n");					// Display lose message
    snprintf (outbuffer, LINESIZE, "Player Has Run Out Of Guesses!\n");				// Set lose message to return to client, with protection against buffer overflow
    write (o, outbuffer, strlen (outbuffer));							// Send outbuf info to client
  }
}

int gameOverSelect(int state, char* outbuffer, char* word, int o, char* clname, int clport) {
  if (state == 'W') {
    printf("Client %s/%d has guessed \"%s\" correctly!\n", clname, clport, word);		// Display win message
    snprintf (outbuffer, LINESIZE, "Player Has Guessed \"%s\" Correctly!\n", word);		// Set win message to return to client
    write (o, outbuffer, strlen (outbuffer));	
    return 1;
  }
  else if (state == 'L') {
    printf("Client %s/%d is a loser!\n", clname, clport);					// Display lose message
    snprintf (outbuffer, LINESIZE, "Player Has Run Out Of Guesses!\n");				// Set lose message to return to client, with protection against buffer overflow
    write (o, outbuffer, strlen (outbuffer));							// Send outbuf info to client
    return 1;
  }
  return 0;
}

// Pick a word at random from the word list, displaying the Client IP & Port
char* selectRandomWord(char* clname, int clport) {  
  char* the_word = word[rand() % NUM_OF_WORDS];							// Select random word from words array for client to guess

  syslog (LOG_USER | LOG_INFO, "server chose hangman word %s", the_word);			// Message logging

  printf("Word: \"%s\" Randomly Selected For Client: %s/%d\n", the_word, clname, clport);	// Display the word selected for the client on the server

  return the_word;										// Return the selected word
}

// Pick a word at random from the word list
char* randomWord() {  
  char* the_word = word[rand() % NUM_OF_WORDS];							// Select random word from words array for client to guess

 // syslog (LOG_USER | LOG_INFO, "server chose hangman word %s", the_word);			// Message logging

  printf("Word: \"%s\" Randomly Selected\n", the_word);						// Display the word selected for the client on the server

  return the_word;										// Return the selected word
}


// Check if a correct guess has been made or not
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


// Display the server hostname
void displayHostname(int o, char* buf) {
  char hostname[LINESIZE];									// Name of the current system

  gethostname (hostname, LINESIZE);								// Get the host name
	
  snprintf(buf, LINESIZE, "Playing hangman on host: %s \n \n", hostname);			// Set outbuf to server hostname, with protection against buffer overflow
  write(o, buf, strlen(buf));									// Send outbuf to client
}


char checkGameState(char* word, char* part, int lives) {
	if (strcmp (word, part) == 0)			// If all letters guessed correctly
		return 'W'; 
	else if (lives <= 0) {			// If client has run out of guesses
		return 'L';
		//strcpy (arrPart[i], arrWords[i]); 	/* User Show the word */
	}
	return 'I';	
}

char initPartWord(char* partWord, int length) {
	int i;											// For loop index
 	for (i = 0; i < length; i++) partWord[i]='-';						// Set every character in the partword to a hyphern
 	
	partWord[i] = '\0';									// Set the string terminator
}

#endif	/* __HANGMAN_H */
