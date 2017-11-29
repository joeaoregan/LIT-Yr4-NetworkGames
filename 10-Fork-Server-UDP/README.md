```c
   _____________          T E A M 1 -  H A N G M A N
   |/      |        Year 4: Networked Games CA - Hangman
   |   ___(")___                  26/11/2017			  Incomplete
   |      |_| 							UDP Fork Server
   |      /^\                    Joe O'Regan
   |    _// \\_                  Jason Foley
 __|___________                 Samantha Marah
```

This is a group assignment for the 4th year Networked Games module. The group consists of:

  * [Joe O'Regan](https://github.com/joeaoregan)
  * [Samantha Marah](https://github.com/jasfoley)
  * [Jason Foley](https://github.com/samanthamarah)

![alt text](https://raw.githubusercontent.com/joeaoregan/Yr4-NetworkGames-Hangman/master/Screenshots/10ForkServerUDP.png "UDP Fork Server")
Unfinished concurrent UDP Server to handle multiple Client connections using fork()

## 10-Fork-Server-UDP (Unfinished)

A concurrent version of the **UDP Server** using `fork()`, to fork a separate process on receiving data with recvfrom to deal with each **Client** connection

###### This socket is incomplete

##### Files:

Code and application included in folder:
| No. | File | Description |
| --- | ------------- |-------------|
| 1 | **serveruf.c** | UDP Fork Server Code |
| 2 | **clientua.c** | TEST: Asynchronous UDP Client Code |
| 3 | **srv** | UDP Fork Server Application |
| 4 | **cli** | TEST: Asynchronous UDP Client Application |

##### Dependencies (Placed in root directory):

Files the UDP Fork Server is dependent on:

1. DrawHangman.h
2. Hangman.h
3. CreateUDPSocket.h
4. Socket.h
5. UDPPlayHangman.h"

## Running:

The included **Asynchronous UDP Client** communicates with the **Server** using `sendto()` and `recvfrom()` (Test Server application srv included in directory)

To run the included compiled **UDP Fork Server** with default port number `1068`:
```c
./srv
```

To run **Server** specifying the port number to listen for **Client** connections on e.g. port `8000`:
```c
./srv 8000

###### With UDP Test Server application running, the Client must use the same port when sending data with sendto()

To run the included compiled **Test Asynchronous UDP Client** with default port number `1068`:
```c
./cli
```

## Compiling:

To compile the **UDP Fork Server** code:
```c
gcc -o srv serveruf.c
```

To compile the included Test code for **UDP Asynchronous Client**:
```c
gcc -o cli clientua.c
```

