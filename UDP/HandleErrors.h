#ifndef	__HANDLE_ERRORS
#define	__HANDLE_ERRORS

#include <stdio.h>	// perror()

// error message function
void displayErrMsg(char *reason) {
	perror(reason);			// Dispay system error message		
	exit(1);			// Exit the program
}

#endif	/* __HANDLE_ERRORS */
