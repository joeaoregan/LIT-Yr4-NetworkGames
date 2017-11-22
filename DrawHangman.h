 /* 
 	File: 		DrawHangman.h
	Version: 	Hangman Graphics
	Author:		Joe O'Regan

	Year 4 Networked Games Assignment

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690

	Mostly unnecessary functions to draw a graphical represenation of the players lives,
	and the game logo displayed when the client and server first start up
	A game over message is also displayed depending on the number of lives/guesses left
*/

#ifndef	__DRAW_HANGMAN_H
#define	__DRAW_HANGMAN_H

#define RED   "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELO  "\x1B[33m"
#define BLUE  "\x1B[34m"
#define CYAN  "\x1B[36m"
#define NORM  "\x1B[0m"

char *hangman[]= {										// ASCII hangman graphic
"   T E A M 1",											// Changed to Team 1 (the actual team name)
" H A N G M A N",
"\x1B[35m  _____________", 
"\x1B[35m  |/      |",
"\x1B[35m  | \x1B[33m  ___(\")___",
"\x1B[35m  | \x1B[33m     |_|   ",
"\x1B[35m  | \x1B[33m     /^\\",
"\x1B[35m  | \x1B[33m   _// \\\\_",
"\x1B[35m__|____________\x1B[0m"};    

// Variables to swap out body parts as the player loses lives
char *altArms[]={"\x1B[35m  |    \x1B[31m_\x1B[33m_(\")___\x1B[0m", 
		 "\x1B[35m  |    \x1B[33m__(\")_\x1B[31m_\x1B[0m", 
		 "\x1B[35m  |      \x1B[33m(\")__\x1B[0m", 
		 "\x1B[35m  |      \x1B[33m(\")"};						// ARMS - left hand, right hand, left arm, none

char *altBody[]={"\x1B[35m  |\x1B[0m"};								// BODY - none

char *altLegs[]={"\x1B[35m  |      \x1B[31m~^\x1B[33m\\",
		 "\x1B[35m  |      \x1B[33m~^~\x1B[0m",};					// LEGS - 0.Left, 1.right, 2.none

char *altFeet[]={"\x1B[35m  |   \x1B[31m  // \x1B[33m\\\\_\x1B[0m",
		 "\x1B[35m  |\x1B[33m     // \x1B[31m\\\\\x1B[0m", 
		 "\x1B[35m  |\x1B[31m        \\\\", 
		 "\x1B[35m  |"};								// Feet - 0.Left foot, 1.right foot, 2.Left shin, 3. Right shin (none)

// Function Declarations
void drawHangman();
Void amputate(int lives);

/*
	CLIENT & SERVER:
	Draw the full hangman intro on Client and Server Sockets
*/
void drawHangman() {
	// Draw hangman
	printf("%s%s\n", RED,hangman[0]);
	printf("%s%s\n", RED,hangman[1]);
	for (int h = 2; h < 9; ++h){								// Draw hangman ASCII
	  printf("%s\n", hangman[h]);
	}
	printf("%sA Game By:\n  * %sJoe O'Regan\n  %s* %sSamantha Marah\n  %s* %sJason Foley%s\n", CYAN,BLUE,CYAN,BLUE,CYAN,BLUE,NORM);	// Game Creators
}
void drawHangmanNew() {
	// Draw hangman
	printf("  %s%s - ", RED,hangman[0]);
	printf("%s%s\n", RED,hangman[1]);
	for (int h = 2; h < 9; ++h){								// Draw hangman ASCII
	  printf("%s", hangman[h]);
	  if (h == 3) printf("%s\t  A Game By:\n",CYAN);
	  else if (h == 4) printf("\t%s  * %sJoe O'Regan\n",CYAN,BLUE);
	  else if (h == 5) printf("\t%s  * %sJason Foley\n",CYAN,BLUE);
	  else if (h == 6) printf("\t%s  * %sSamantha Marah%s\n",CYAN,BLUE,NORM);
	  else printf("\n");
	}
	//printf("%sA Game By:\n  * %sJoe O'Regan\n  %s* %sSamantha Marah\n  %s* %sJason Foley%s\n", CYAN,BLUE,CYAN,BLUE,CYAN,BLUE,NORM);	// Game Creators
}

/*
	CLIENT:
	This function displays the graphical version of the lives remaining
	First parsing the string from the server, to separate the part word and number of lives left
*/
void amputate(int lives) {									// Takes a parameter of the number of lives/guesses remaining to draw the hangman game
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
	    if (h == 4) printf("\x1B[35m  |       \x1B[33mO\x1B[35m\n");
	    else if (h == 5 || h == 6 || h == 7) printf("%s\n", altBody[0]);
	    else printf("%s\n", hangman[h]);	
	  }
	  printf("Now you have actually lost your mind!\n");
	}
}


/*
	CLIENT:
	Select the game over message to display 
	based on the number of lives/guesses the Player had left at the end of the game
*/
void gameOverText(int win) {
	if (win == 12) printf("%sPERFECT ROUND\nGAME OVER!%s\n", BLUE, NORM);			// Game over message to test prog terminates, perfect score
	else if (win == 0) printf("%sLOSER! GAME OVER!%s\n", RED, NORM);			// Game over message, player ran out of guesses
	else printf("%sPLAYER WINS\nGAME OVER!%s\n", GREEN, NORM);				// Game over message, could do better
}


#endif	/* __DRAW_HANGMAN_H */
