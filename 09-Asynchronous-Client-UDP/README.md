```c
   _____________          T E A M 1 -  H A N G M A N
   |/      |        Year 4: Networked Games CA - Hangman
   |   ___(")___                  26/11/2017			Asynchronous
   |      |_| 							 UDP Client
   |      /^\                    Joe O'Regan
   |    _// \\_                  Jason Foley
 __|___________                 Samantha Marah
```

This is a group assignment for the 4th year Networked Games module. The group consists of:

  * [Joe O'Regan](https://github.com/joeaoregan)
  * [Samantha Marah](https://github.com/jasfoley)
  * [Jason Foley](https://github.com/samanthamarah)

![alt text](https://raw.githubusercontent.com/joeaoregan/Yr4-NetworkGames-Hangman/master/Screenshots/9AsynchronousClientUDP.png "Asynchronous UDP Client")
UDP Client handling asynchronous inputs using select() 

## 9-Asynchronous-Client-UDP

An **Asynchronous UDP Client** that can handle input asynchronously from socket and file descriptors using `select()`

##### Files:

Code and application included in folder:
|  | File        | Description           |
| --- | ------------- |-------------|
| 1. | **clientua.c** | Asynchronous UDP Client code |
| 2. | **serverua.c** | TEST: UDP Server code |
| 3. | **cli** | synchronous UDP Client application |
| 4. | **srv** | TEST: UDP Server application |

##### Dependencies (Placed in root directory):
Files the **Asynchronous UDP Client** is dependent on:

1. DrawHangman.h
2. Hangman.h
3. CreateUDPSocket.h

## Running:

The **Asynchronous UDP Client** communicates with the **Server** using `sendto()` and `recvfrom()` (Test Server application srv included in directory)

To run the included compiled **Test Server** with default port number `1068` (Different to TCP `1066`):
```c
./srv
```

To run **Server** specifying the port number to listen for **Client** connections on e.g. port `8000`:
```c
./srv 8000
```

###### With UDP Test Server application running, the Client must use the same port when sending data with sendto()


The already compiled **Asynchronous UDP Client** application included in the directory can be run with commands: 

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

To compile the **Asynchronous UDP Client** code:
```c
gcc -o cli clientua.c
```

