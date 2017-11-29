```c
   _____________          T E A M 1 -  H A N G M A N
   |/      |        Year 4: Networked Games CA - Hangman
   |   ___(")___                  26/11/2017			Concurrent Server
   |      |_| 							   Using fork()
   |      /^\                    Joe O'Regan
   |    _// \\_                  Jason Foley
 __|___________                 Samantha Marah
```

This is a group assignment for the 4th year Networked Games module. The group consists of:
  * [Joe O'Regan](https://github.com/joeaoregan)
  * [Samantha Marah](https://github.com/jasfoley)
  * [Jason Foley](https://github.com/samanthamarah)

![alt text](https://raw.githubusercontent.com/joeaoregan/Yr4-NetworkGames-Hangman/master/Screenshots/4ForkServerTCP.png "Concurrent Server using fork()")
Concurrent Server using fork() to handle multiple Client connections

##  04. Fork Server TCP

A concurrent version of the **TCP Server** using `fork()`, to fork a separate process to deal with each **Client** connection

##### Files:

Code and application included in folder:
| --- | File        | Description           |
| --- | ------------- |-------------|
| 1. | **serverf.c** | Concurrent Forking Server Code |
| 2. | **srv** | Concurrent Forking Server Application |
| 3. | **cli** | Test: Client Application |

##### Dependencies (Placed in root directory):
Files that the client is dependent on to compile:

1. DrawHangman.h
2. Hangman.h
3. CreateTCPSocket.h
4. Socket.h
5. TCPPlayHangman.h

## Running:

This **Server** application will communicate with the included **Client** application (cli) from 2-Hangman-Modified-TCP

To run the existing compiled **Forking Server** with default port number `1066`:
```c
./srv
```
To run **Server** specifying the port number to listen for **Client** connections on e.g. port `8000`:
```c
./srv 8000
```

####### With Forking Server application running, the Client must connect to the same port
The already compiled **Client** application included in the directory can be run with command: 
```c
./srv
```
Or see README.md in 02-Hangman-Modified-TCP for further **Client** running instructions

## Compiling:

To compile the **Forking Server** code (with the dependent files in parent directory):
```c
gcc -o srv serverf.c
```

