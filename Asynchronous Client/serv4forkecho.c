//tcpserv04

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>					// printf()
#include <errno.h>
#include <string.h>					// 23/09/2017 Warning for strcpy, bzero()
#include <stdlib.h>					// 23/09/2017 Warning for exit
#include <unistd.h>					// 23/09/2017 gethostname(), write(), read(), close()
#include <arpa/inet.h>					// 23/09/2017 inet_ntop()
#include "../HandleErrors.h"
#include "../Hangman.h"
#include <signal.h>					// SIGCHLD
#include <sys/types.h>					// waitpid()
#include <sys/wait.h>

#define SERV_PORT 1066
#define LISTENQ 5


void str_echo(int sockfd);
void sig_chld(int signo);

int main(int argc, char **argv) {
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	void sig_chld(int);

	//listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0) displayErrMsg("Creating Stream Socket");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	//Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
	if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) displayErrMsg("Bind()");

	//Listen(listenfd, LISTENQ);
	listen(listenfd, LISTENQ);

	//Signal(SIGCHLD, sig_chld);							/* must call waitpid() */
	signal(SIGCHLD, sig_chld);							/* must call waitpid() */

	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		//if ( (connfd = accept(listenfd, (SA *) &cliaddr, &clilen)) < 0) {
		if ( (connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) < 0) {
			if (errno == EINTR) continue;						/* back to for() */
			//else err_sys("accept error");
			else displayErrMsg("accept error");
		}

		//if ( (childpid = Fork()) == 0) {					/* child process */
		if ( (childpid = fork()) == 0) {					/* child process */
			//Close(listenfd);						/* close listening socket */
			close(listenfd);						/* close listening socket */
			str_echo(connfd);						/* process the request */
			exit(0);
		}

		//Close(connfd);							/* parent closes connected socket */
		close(connfd);								/* parent closes connected socket */
	}
}

void str_echo(int sockfd) {
	ssize_t		n;
	char		buf[LINESIZE];

again:
	while ( (n = read(sockfd, buf, LINESIZE)) > 0)
		//Writen(sockfd, buf, n);
		write(sockfd, buf, n);

	if (n < 0 && errno == EINTR) goto again;
	//else if (n < 0) err_sys("str_echo: read error");
	else if (n < 0) displayErrMsg("str_echo: read error");
}

void sig_chld(int signo) {
	pid_t	pid;
	int stat;

	while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0) 
		printf("child %d terminated\n", pid);
	return;
}
