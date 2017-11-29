```c
   _____________          T E A M 1 -  H A N G M A N
   |/      |        Year 4: Networked Games CA - Hangman	
   |   ___(")___                  26/11/2017			Asyncronous
   |      |_| 							TCP Client
   |      /^\                    Joe O'Regan
   |    _// \\_                  Jason Foley
 __|___________                 Samantha Marah
```

This is a group assignment for the 4th year Networked Games module. The group consists of:
  * [Joe O'Regan](https://github.com/joeaoregan)
  * [Samantha Marah](https://github.com/jasfoley)
  * [Jason Foley](https://github.com/samanthamarah)

![alt text](https://raw.githubusercontent.com/joeaoregan/Yr4-NetworkGames-Hangman/master/Screenshots/7AsynchronousClientTCP.png "Asynchronous TCP Client")
TCP Client handling asynchronous inputs using select()


## 7-Asynchronous-Client-TCP

The **Asynchronous TCP Client** handles input asynchronously from the socket and file descriptors using the `select()` function

##### Files:

Code and application included in folder:
|  | File        | Description           |
| --- | ------------- |-------------|
| 1. | **clientat.c** | Asynchronous TCP Client Code |
| 2. | **cla** | Asynchronous Client Application |
| 3. | **srvf** | Forking Server Application (04-Fork-Server-TCP) |

##### Dependencies (Placed in root directory):
Files the Asynchronous Client is dependent on:

1. DrawHangman.h
2. Hangman.h
3. CreateTCPSocket.h

## Running:

The **Asynchronous TCP Client** communicates with the **Server** using `read()` and `write()` (Forked Server application srvf included in directory)

To run the included compiled **Fork Server** with default port number `1066`:
```c
./srv
```

To run **Server** specifying the port number to listen for client connections on e.g. port `8000`:
```c
./srv 8000
```

###### With TCP Server application running, the Client must use the same port when sending data


The already compiled **Asynchronous TCP Client** application included in the directory can be run with commands: 

```c
./cla
```

To run the **Client** specifying an IP Address `193.168.1.10` and default port: 
```c
./cla 193.168.1.10
```

To run the **Client** specifying an IP Address `193.168.1.10` and port `8000`: 
```c
./cla 193.168.1.10 8000
```

The **Client** can also connect using `localhost`
```c
./cla localhost
```

And the **Client** can also connect specifying the name of a PC e.g. `Joe-PC`
```c
./cla Joe-PC
```

## Compiling:

To compile the **Asynchronous TCP Client** code:
```c
gcc -o cla clientat.c
```

