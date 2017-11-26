#ifndef	__GAME_STATE
#define	__GAME_STATE

/*
	Data structure containing the variable necessary to retain game state
*/
struct sessionData {						// Store the game state, (XXX maybe use a linked list of structs instead of array)
	int sock;						// The client socket that is connected
	char* word;						// Store the current full word
	char partWord[LINESIZE];				// Store the current part word
	int lives;						// Store the number of guesses left for each client
	int gameState;						// Store the game state for the connection
	char bufIn[LINESIZE];					// Input Buffer
	char bufOut[LINESIZE];					// Output Buffer
	char ip[INET_ADDRSTRLEN];				// client IP
	int port;						// client port
};


/*
	Reset or clear sessionData variables when a Client has finished playing
*/
void closeSocketConnection(struct sessionData* cli) {
	(*cli).sock = -1;					// Clear client from client index
	(*cli).word = "";					// Whole word for each client
	(*cli).partWord[0] = '\0';				// Part word for each client
	(*cli).lives = MAX_LIVES;				// Lives for each client
	(*cli).gameState = 'I';					// Game state for each client
	(*cli).bufIn[0] = '\0';					// Clear client from client index
}

/*
	Display the current game state
*/
void displayState(int i, char* word, char* part, int lives, char state) {
	printf("\nState %d Word: %s partWord: %s ",		// Display Clients:
		i,word,part);					// Position in Clients array, word to guess, part word
	printf("lives %d gameState %c\n\n",
		lives,state);					// Number of guesses/lives left, current game state
}

/*
	Initialise the game state for a client
*/
void initClient(struct sessionData* cli) {
//	printf("Init the game state\n");
	(*cli).word = selectRandomWord((*cli).ip, (*cli).port);	// Whole word for each client
	initPartWord((*cli).partWord, strlen((*cli).word));	// Part word for each client
	(*cli).lives = MAX_LIVES;				// Lives for each client
	(*cli).gameState = 'I';					// Game state for each client
}

#endif	/* __GAME_STATE */
