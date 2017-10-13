#ifndef	__SOCKET_FUNCTIONS_H
#define	__SOCKET_FUNCTIONS_H

void displayConnectedClientInfo() {
  if (inet_ntop(AF_INET, &client.sin_addr.s_addr, clntName,sizeof(clntName)) != NULL){
    printf("Handling client %s/%d \n", clntName, CLI_PORT);
  }
}

#endif	/* __SOCKET_FUNCTIONS_H */
