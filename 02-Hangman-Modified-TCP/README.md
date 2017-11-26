```c
   _____________          T E A M 1 -  H A N G M A N
   |/      |        Year 4: Networked Games CA - Hangman
   |   ___(")___                  26/11/2017			  Modified
   |      |_| 							Hangman Game
   |      /^\                    Joe O'Regan
   |    _// \\_                  Jason Foley
 __|___________                 Samantha Marah
```

This is a group assignment for the 4th year Networked Games module. The group consists of:
  * [Joe O'Regan](https://github.com/joeaoregan)
  * [Samantha Marah](https://github.com/jasfoley)
  * [Jason Foley](https://github.com/samanthamarah)

![alt text](https://raw.githubusercontent.com/joeaoregan/Yr4-NetworkGames-Hangman/master/Screenshots/2ModifiedHangman.png "Modified Hangman Client and Server")

## 02-Hangman-Modified-TCP

**Modified Hangman Game**, with graphics, coloured text, and functionality to set up sockets, and play the game separated to header files

##### Files:

1. clientm.c (Client code)
2. serverm.c (Server code)
3. cli (Client Application)
4. srv (Server Application)

##### Dependencies (Placed in root directory):
Files that the client is dependent on to compile:

1. DrawHangman.h
2. Hangman.h
3. CreateTCPSocket.h

Additional **Server** dependencies:

1. Socket.h
2. TCPPlayHangman.h

## Running:

To run the existing compiled **Server** with default port number `1066`:
```c
./srv
```
To run **Server** specifying the port number to listen for **Client** connections on e.g. port `8000`:
```c
./srv 8000
```

###### With Server application running, the Client must connect to the same port

To run the existing compiled **Client** with default IP Address and default port number: 
```c
./cli
```

To run the **Client** specifying an IP Address `193.168.1.10` and default port: 
```c
./cli 193.168.1.10
```

To run the **Client** specifying an IP Address `193.168.1.10` and port `8000` (Client port must match Server): 
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

To compile the **Client** code (with the dependent files in parent directory):
```c
gcc -o cli clientm.c
```

To compile the **Server** code:
```c
gcc -o srv serverm.c
```

Where `cli` and `srv` are the names of the compiled programs
