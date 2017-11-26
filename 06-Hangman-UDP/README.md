```c
   _____________          T E A M 1 -  H A N G M A N
   |/      |        Year 4: Networked Games CA - Hangman
   |   ___(")___                  26/11/2017			UDP Server
   |      |_| 							And Client
   |      /^\                    Joe O'Regan
   |    _// \\_                  Jason Foley
 __|___________                 Samantha Marah
```

This is a group assignment for the 4th year Networked Games module. The group consists of:
  * [Joe O'Regan](https://github.com/joeaoregan)
  * [Samantha Marah](https://github.com/jasfoley)
  * [Jason Foley](https://github.com/samanthamarah)

![alt text](https://raw.githubusercontent.com/joeaoregan/Yr4-NetworkGames-Hangman/master/Screenshots/6HangmanUDP.png "UDP Server and Client")

## 6-Hangman-UDP

A Hangman **UDP Client** and **UDP Server** using `sendto()` and `recvfrom()` to communicate between each other in a connectionless state over `UDP`

##### Files:

1. serveru.c (UDP Server code)
2. clientu.c (UDP Client code)
3. srv (UDP Server Application)
4. cli (UDP Client Application)

##### Dependencies (Placed in root directory):
Files that the Select Server is dependent on to compile:

1. DrawHangman.h
2. Hangman.h
3. CreateUDPSocket.h
4. Socket.h
5. UDPPlayHangman.h

Files the **UDP Client** (Modified to work with the **UDP Server**) is dependent on:*

0. None 

## Running:

This **UDP Server** application will communicate with the included UDP **Client** application using `sendto()` and `recvfrom()`

To run the existing compiled **Select Server** with default port number `1068` (Different to TCP `1066`):
```c
./srv
```

To run **Server** specifying the port number to listen for **Client** connections on e.g. port `8000`:
```c
./srv 8000
```

###### With UDP Server application running, the Client must use the same port when sending data


The already compiled **Client** application included in the directory can be run with commands: 

```c
./cli
```

To run the **Client** specifying an IP Address `193.168.1.10` and default port: 
```c
./cli 193.168.1.10
```

To run the **Client** specifying an IP Address `193.168.1.10` and port `8000`: 
```c
./cli 193.168.1.10 8000
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

To compile the **UDP Server** code (with the dependent files in parent directory):
```c
gcc -o srv serveru.c
```

To compile the **UDP Client** code:
```c
gcc -o cli clientu.c
```

