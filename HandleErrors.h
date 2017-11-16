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
