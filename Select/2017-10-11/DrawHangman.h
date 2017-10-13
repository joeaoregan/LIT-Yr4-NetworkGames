/* 
	DrawHangman.h
*/

#ifndef	__DRAW_HANGMAN_H
#define	__DRAW_HANGMAN_H

char *hangman[]= {						// ASCII hangman graphic
"   T E A M 1",							// Changed to Team 1 (the actual team name)
" H A N G M A N",
"  _____________", 
"  |/      |",
"  |   ___(\")___",
"  |      |_|",
"  |      /^\\",
"  |    _// \\\\_",
"__|____________"};    

char *amputee[7];


char *altArms[]={"  |    __(\")___", "  |    __(\")__", "  |      (\")__", "  |      (\")"};	// ARMS - left hand, right hand, left arm, none
char *altBody[]={"  |"};									// BODY - none
char *altLegs[]={"  |      ~^\\", "  |      ~^~",};						// LEGS - 0.Left, 1.right, 2.none
char *altFeet[]={"  |     // \\\\_","  |     // \\\\", "  |        \\\\", "  |"};		// Feet - 0.Left foot, 1.right foot, 2.Left shin, 3. Right shin (none)

void drawHangman();
void amputate(int lives);

void drawHangman() {
	// Draw hangman
	for (int h = 0; h < 9; ++h){				// Draw hangman ASCII
	  printf("%s\n", hangman[h]);
	}
}

void amputate(int lives) {
	if (lives == 12) {					// Full
	  for (int h = 2; h < 9; ++h) {
	    printf("%s\n", hangman[h]);	    
	  }
	  //printf("test: ");
	}
	if (lives == 11) {					// Left Foot
	  for (int h = 2; h < 9; ++h){
	    if (h == 7) printf("%s\n", altFeet[0]);
	    else printf("%s\n", hangman[h]);
	  }
	}
	if (lives == 10) {					// Right Foot
	  for (int h = 2; h < 9; ++h){			
	    if (h == 7) printf("%s\n", altFeet[1]);
	    else printf("%s\n", hangman[h]);
	  }
	}
	if (lives == 9) {					// Left Shin
	  for (int h = 2; h < 9; ++h){	
	    if (h == 7) printf("%s\n", altFeet[2]);
	    else printf("%s\n", hangman[h]);
	  }
	}
	if (lives == 8) {					// Right Shin
	  for (int h = 2; h < 9; ++h){				
	    if (h == 7) printf("%s\n", altFeet[3]);
	    else printf("%s\n", hangman[h]);
	  }
	}



	if (lives == 7) {					// Left Tight
	  for (int h = 2; h < 9; ++h){
	    if (h == 6) printf("%s\n", altLegs[0]);
	    else if (h == 7) printf("%s\n", altFeet[3]);
	    else printf("%s\n", hangman[h]);	
	  }
	}
	if (lives == 6) {					// Right Tight
	  for (int h = 2; h < 9; ++h){
	    if (h == 6) printf("%s\n", altLegs[1]);
	    else if (h == 7) printf("%s\n", altFeet[3]);
	    else printf("%s\n", hangman[h]);	
	  }
	}


	if (lives == 5) {					// Body
	  for (int h = 2; h < 9; ++h){
	    if (h == 5) printf("%s\n", altBody[0]);
	    else if (h == 6 || h == 7) printf("%s\n", altFeet[3]);
	    else printf("%s\n", hangman[h]);	
	  }
	}


	if (lives == 4) {					// Left Hand
	  for (int h = 2; h < 9; ++h){
	    if (h == 4) printf("%s\n", altArms[0]);
	    else if (h == 5 || h == 6 || h == 7) printf("%s\n", altBody[0]);
	    else printf("%s\n", hangman[h]);	
	  }
	}
	if (lives == 3) {					// Right Hand
	  for (int h = 2; h < 9; ++h){
	    if (h == 4) printf("%s\n", altArms[1]);
	    else if (h == 5 || h == 6 || h == 7) printf("%s\n", altBody[0]);
	    else printf("%s\n", hangman[h]);	
	  }
	}
	if (lives == 2) {					// Left Arm
	  for (int h = 2; h < 9; ++h){
	    if (h == 4) printf("%s\n", altArms[2]);
	    else if (h == 5 || h == 6 || h == 7) printf("%s\n", altBody[0]);
	    else printf("%s\n", hangman[h]);	
	  }
	}
	if (lives == 1) {					// Right Arm
	  for (int h = 2; h < 9; ++h){
	    if (h == 4) printf("%s\n", altArms[3]);
	    else if (h == 5 || h == 6 || h == 7) printf("%s\n", altBody[0]);
	    else printf("%s\n", hangman[h]);	
	  }
	}


	if (lives == 0) {					// Head
	  for (int h = 2; h < 9; ++h){
	    if (h == 4) printf("  |       O\n");
	    else if (h == 5 || h == 6 || h == 7) printf("%s\n", altBody[0]);
	    else printf("%s\n", hangman[h]);	
	  }
	}
}

#endif	/* __DRAW_HANGMAN_H */
