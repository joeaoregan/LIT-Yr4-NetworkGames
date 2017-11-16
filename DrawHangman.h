/* 
	DrawHangman.h

	Functions to draw a graphical represenation of the players lives,
	and the game logo displayed when the client and server first start up
*/

#ifndef	__DRAW_HANGMAN_H
#define	__DRAW_HANGMAN_H

char *hangman[]= {										// ASCII hangman graphic
"   T E A M 1",											// Changed to Team 1 (the actual team name)
" H A N G M A N",
"  _____________", 
"  |/      |",
"  |   ___(\")___",
"  |      |_|",
"  |      /^\\",
"  |    _// \\\\_",
"__|____________"};    

// Variables to swap out body parts as the player loses lives
char *altArms[]={"  |    __(\")___", "  |    __(\")__", "  |      (\")__", "  |      (\")"};	// ARMS - left hand, right hand, left arm, none
char *altBody[]={"  |"};									// BODY - none
char *altLegs[]={"  |      ~^\\", "  |      ~^~",};						// LEGS - 0.Left, 1.right, 2.none
char *altFeet[]={"  |     // \\\\_","  |     // \\\\", "  |        \\\\", "  |"};		// Feet - 0.Left foot, 1.right foot, 2.Left shin, 3. Right shin (none)

// Function Declarations
void drawHangman();
//void amputate(int lives);

// Draw the full hangman intro on Client and Server Sockets
void drawHangman() {
	// Draw hangman
	for (int h = 0; h < 9; ++h){								// Draw hangman ASCII
	  printf("%s\n", hangman[h]);
	}
	printf("A Game By:\n  * Joe O'Regan\n  * Samantha Marah\n  * Jason Foley\n");		// Game Creators
}


// This function displays the graphical version of the lives remaining
// First parsing the string from the server, to separate the part word and number of lives left
//void amputate(int lives) {
void amputate(char* input) {
	char partWord[20];
	int lives;

	sscanf(input, "%s %d", &(*partWord), &lives);						// Parse string data received from server into separate part-word and score variables

	printf("\n");										// Start on new line
	if (lives == 12) {									// Full
	  for (int h = 2; h < 9; ++h) {
	    printf("%s\n", hangman[h]);	    
	  }
	  printf("As if hanging wasn't bad enough\nYou are also facing the threat of amputation!\n");
	}
	if (lives == 11) {									// Left Foot
	  for (int h = 2; h < 9; ++h){
	    if (h == 7) printf("%s\n", altFeet[0]);
	    else printf("%s\n", hangman[h]);
	  }
	  printf("You lost your left foot, start hopping!\n");
	}
	if (lives == 10) {									// Right Foot
	  for (int h = 2; h < 9; ++h){			
	    if (h == 7) printf("%s\n", altFeet[1]);
	    else printf("%s\n", hangman[h]);
	  }
	  printf("You lost your right foot\nNo more shoes for you!\n");
	}
	if (lives == 9) {									// Left Shin
	  for (int h = 2; h < 9; ++h){	
	    if (h == 7) printf("%s\n", altFeet[2]);
	    else printf("%s\n", hangman[h]);
	  }
	  printf("You lost your left shin\n");
	}
	if (lives == 8) {									// Right Shin
	  for (int h = 2; h < 9; ++h){				
	    if (h == 7) printf("%s\n", altFeet[3]);
	    else printf("%s\n", hangman[h]);
	  }
	  printf("You lost your right shin\n");
	}


	if (lives == 7) {									// Left Tight
	  for (int h = 2; h < 9; ++h){
	    if (h == 6) printf("%s\n", altLegs[0]);
	    else if (h == 7) printf("%s\n", altFeet[3]);
	    else printf("%s\n", hangman[h]);	
	  }
	  printf("You lost your left tight\n");
	}
	if (lives == 6) {									// Right Tight
	  for (int h = 2; h < 9; ++h){
	    if (h == 6) printf("%s\n", altLegs[1]);
	    else if (h == 7) printf("%s\n", altFeet[3]);
	    else printf("%s\n", hangman[h]);	
	  }
	  printf("You won't be needing pants anymore!\n");
	}


	if (lives == 5) {									// Body
	  for (int h = 2; h < 9; ++h){
	    if (h == 5) printf("%s\n", altBody[0]);
	    else if (h == 6 || h == 7) printf("%s\n", altFeet[3]);
	    else printf("%s\n", hangman[h]);	
	  }
	  printf("You lost a few inches around the waste!\n");
	}


	if (lives == 4) {									// Left Hand
	  for (int h = 2; h < 9; ++h){
	    if (h == 4) printf("%s\n", altArms[0]);
	    else if (h == 5 || h == 6 || h == 7) printf("%s\n", altBody[0]);
	    else printf("%s\n", hangman[h]);	
	  }
	  printf("You were right handed ya?\n");
	}
	if (lives == 3) {									// Right Hand
	  for (int h = 2; h < 9; ++h){
	    if (h == 4) printf("%s\n", altArms[1]);
	    else if (h == 5 || h == 6 || h == 7) printf("%s\n", altBody[0]);
	    else printf("%s\n", hangman[h]);	
	  }
	  printf("Wave goodbye to your right hand!\n");
	}
	if (lives == 2) {									// Left Arm
	  for (int h = 2; h < 9; ++h){
	    if (h == 4) printf("%s\n", altArms[2]);
	    else if (h == 5 || h == 6 || h == 7) printf("%s\n", altBody[0]);
	    else printf("%s\n", hangman[h]);	
	  }
	  printf("Now your wheelchair will be going in circles!\n");
	}
	if (lives == 1) {									// Right Arm
	  for (int h = 2; h < 9; ++h){
	    if (h == 4) printf("%s\n", altArms[3]);
	    else if (h == 5 || h == 6 || h == 7) printf("%s\n", altBody[0]);
	    else printf("%s\n", hangman[h]);	
	  }
	  printf("You weren't a sleeves kind of person anyway!\n");
	}


	if (lives == 0) {									// Head
	  for (int h = 2; h < 9; ++h){
	    if (h == 4) printf("  |       O\n");
	    else if (h == 5 || h == 6 || h == 7) printf("%s\n", altBody[0]);
	    else printf("%s\n", hangman[h]);	
	  }
	  printf("Now you have actually lost your mind!\n");
	}

	printf("\nRemaining Guesses:\t%d\n", lives);						// Guesses remaining
	if (lives > 0) printf("Word To Guess:\t\t%s\n", partWord);				// The part word string while game is playing OR
	else printf("The word was:\t\t%s\n", partWord);						// The actual word if player loses
}


void selectLives(int lives) {
	printf("\nRemaining Guesses:\t%d\n", lives);						// Guesses remaining
	if (lives > 0) printf("Word To Guess:\t\t");						// The part word string while game is playing OR
	else printf("The word was:\t\t");							// The actual word if player loses
}

#endif	/* __DRAW_HANGMAN_H */
