```c
   _____________          T E A M 1 -  H A N G M A N
   |/      |        Year 4: Networked Games CA - Hangman
   |   ___(")___                  26/11/2017			Concurrent Server
   |      |_| 							 Using select()
   |      /^\                    Joe O'Regan
   |    _// \\_                  Jason Foley
 __|___________                 Samantha Marah
```

This is a group assignment for the 4th year Networked Games module. The group consists of:
  * [Joe O'Regan](https://github.com/joeaoregan)
  * [Samantha Marah](https://github.com/jasfoley)
  * [Jason Foley](https://github.com/samanthamarah)

![alt text](https://raw.githubusercontent.com/joeaoregan/Yr4-NetworkGames-Hangman/master/Screenshots/5HangmanSelectTCP.png "Select Server Handling 2 Clients")
Concurrent Server using select() to handle multiple Client connections

## 5-Hangman-Select-TCP

A concurrent version of the **TCP Server** using `select()`, to handle input from different **Client** connections by one **Server** process

##### Files:

Code and application included in folder:

| No. | File | Description |
| --- | --- | --- |
| 1 | **servers.c** | Concurrent Select Server Code |
| 2 | **client.c** | Test: Client Code |
| 3 | **srv** | Concurrent Select Server Application |
| 4 | **cli** | Test: Client Application |

##### Dependencies (Placed in root directory):
Files that the **Select Server** is dependent on to compile:

1. DrawHangman.h
2. Hangman.h
3. CreateTCPSocket.h
4. Socket.h
5. TCPPlayHangman.h
6. GameState.h

Files the **Client** (Modified to work with the Select Server) is dependent on:

0. None 

## Running:

This concurrent **Select Server** application will communicate with the included **Client** application

To run the existing compiled **Select Server** with default port number `1066`:
```c
./srv
```

To run **Server** specifying the port number to listen for **Client** connections on e.g. port `8000`:
```c
./srv 8000
```

###### With Select Server application running, the Client must connect to the same port


The already compiled **Client** application included in the directory can be run with commands:
```c
./cli
```

To run the **Client** specifying an IP Address `193.168.1.10` and default port: 
```c
./cli 193.168.1.10
```

To run the **Client** specifying an IP Address `193.168.1.10` and port `5000`: 
```c
./cli 193.168.1.10 5000
```

The **Client** can also connect using `localhost`
```c
./cli localhost
```

And the **Client** can also connect specifying the name of a PC e.g. `Joe-PC`
```c
./cli Joe-PC
```

## Compiling:

To compile the **Select Server** code (with the dependent files in parent directory):
```c
gcc -o srv servers.c
```

To compile the **Select Client** code:
```c
gcc -o cli clients.c
```

