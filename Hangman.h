#ifndef	__HANGMAN_H
#define	__HANGMAN_H

#include <syslog.h>

#define	MAX_CLIENTS 5										/* 2nd argument to listen() */
#define	MAXLINE	4096										/* max text line length */

#define LINESIZE 80										/* Maximum size in the world of Any string */
#define HANGMAN_TCP_PORT 1066

char* TCP_PORT = "1066";

char* word [] = {
  #include "words"
};

#define NUM_OF_WORDS (sizeof (word) / sizeof (word [0]))

#define MAX_LIVES 12

void play_hangman(int in, int out);								// Function delcaration for play_hangman

// Display end of game message if game complete
void checkGameOver(int state, char* outbuffer, char* word, int o, char* clname, int clport) {
  if (state == 'W') {
  //printf("checkGameOver(): arrStates %c outbuffer %s\n", state, outbuffer);
    printf("Client %s/%d has guessed \"%s\" correctly!\n", clname, clport, word);
   // snprintf (outbuffer, sizeof(outbuffer), "\nPlayer Has Guessed \"%s\" Correctly!\n", word);// Set win message to return to client
    snprintf (outbuffer, LINESIZE, "Player Has Guessed \"%s\" Correctly!\n", word);		// Set win message to return to client
    write (o, outbuffer, strlen (outbuffer));							// Send outbuf info to client
  }
  else if (state == 'L') {
  //printf("checkGameOver(): arrStates %c outbuffer %s\n", state, outbuffer);
    printf("Client %s/%d is a loser!\n", clname, clport);
    //snprintf (outbuffer, sizeof(outbuffer), "\nPlayer Has Run Out Of Guesses!\n");		// Set lose message to return to client, with protection against buffer overflow
    snprintf (outbuffer, LINESIZE, "Player Has Run Out Of Guesses!\n");				// Set lose message to return to client, with protection against buffer overflow
    write (o, outbuffer, strlen (outbuffer));							// Send outbuf info to client
  }
}

// Pick a word at random from the word list
char* selectRandomWord(char* clname, int clport) {  
  char* the_word;

  the_word = word[rand() % NUM_OF_WORDS];							// Select random word from words array
  syslog (LOG_USER | LOG_INFO, "server chose hangman word %s", the_word);			// Message logging

  printf("Word: \"%s\" Randomly Selected For Client: %s/%d\n", the_word, clname, clport);

  return the_word;
}

// Check if a correct guess has been made or not
int correctGuess(char* word, char* part, char* guess) {
  int i, good_guess = 0;									// Set/reset good_guess to false
    for (i = 0; i < strlen(word); i++) {
      if (guess [0] == word [i]) {								// If the guess from client is a match
        good_guess = 1;										// Set good guess true
        part[i] = word[i];									// Fill the guessed letter in correct position
      }
    }

  return good_guess;		
}

// Display the server hostname
void displayHostname(int o, char* buf) {
  char hostname[LINESIZE];									// Name of the current system

  gethostname (hostname, LINESIZE);								// Get the host name
	
  //snprintf(buf, sizeof(buf), "Playing hangman on host: %s \n \n", hostname);			// Set outbuf to server hostname, with protection against buffer overflow
  snprintf(buf, LINESIZE, "Playing hangman on host: %s \n \n", hostname);			// Set outbuf to server hostname, with protection against buffer overflow
  write(o, buf, strlen(buf));									// Send outbuf to client
}

#endif	/* __HANGMAN_H */
