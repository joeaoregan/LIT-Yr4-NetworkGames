```c
   _____________          T E A M 1 -  H A N G M A N
   |/      |        Year 4: Networked Games CA - Hangman
   |   ___(")___                  26/11/2017			Modified Client
   |      |_| 							     using
   |      /^\                    Joe O'Regan			 send() recv()
   |    _// \\_                  Jason Foley
 __|___________                 Samantha Marah
```

This is a group assignment for the 4th year Networked Games module. The group consists of:
  * [Joe O'Regan](https://github.com/joeaoregan)
  * [Samantha Marah](https://github.com/jasfoley)
  * [Jason Foley](https://github.com/samanthamarah)

![alt text](https://raw.githubusercontent.com/joeaoregan/Yr4-NetworkGames-Hangman/master/Screenshots/3ModifiedClientSendRecv.png "Modified Client using send() and recv()")

## 3. Client Send Recv TCP

The Modified **Client** using `send()` and `recv()` instead of read() and write() to communicate with the server

##### Files:

1. clientsr.c (code)
2. cli (Client Application)
3. srv (2-Hangman-Modified-TCP Server Application)

##### Dependencies (Placed in root directory):
Files that the **Client** is dependent on to compile:

1. DrawHangman.h
2. Hangman.h
3. CreateTCPSocket.h

## Running:

This **Client** application will communicate with the **Server** application in 2-Hangman-Modified-TCP

####### With Server application running, the Client must connect to the same port

To run the existing compiled **Client** with default IP Address and default port number: 
```c
./cli
```

To run the **Client** specifying an IP Address `193.168.1.10` and default port: 
```c
./cli 193.168.1.10
```

To run the **Client** specifying an IP Address `193.168.1.10` and port `8000` (Same as Server): 
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
gcc -o cli clientsr.c
```

