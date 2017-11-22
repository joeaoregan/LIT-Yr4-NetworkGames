 /* 
 	File: 		HandleErrors.h
	Version: 	Error handling functions, with and without setting status
	Author:		Joe O'Regan

	Year 4 Networked Games Assignment

	Team 1:
	Joe O'Regan 	K00203642
	Samantha Marah	K00200782
	Jason Foley 	K00186690
*/

#ifndef	__HANDLE_ERRORS
#define	__HANDLE_ERRORS

#include <stdio.h>	// perror()

// error message function
void displayErrMsg(char *reason) {
	perror(reason);			// Dispay system error message		
	exit(1);			// Exit the program
}

// Error message function with exit status
void displayErrMsgStatus(char *reason, int status) {
	perror(reason);			// Dispay system error message		
	exit(status);			// Exit the program
}

#endif	/* __HANDLE_ERRORS */
