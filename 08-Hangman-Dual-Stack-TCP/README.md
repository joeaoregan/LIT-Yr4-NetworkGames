```c
   _____________          T E A M 1 -  H A N G M A N
   |/      |        Year 4: Networked Games CA - Hangman
   |   ___(")___                  26/11/2017			   Dual Stack
   |      |_| 							Client and Server
   |      /^\                    Joe O'Regan
   |    _// \\_                  Jason Foley
 __|___________                 Samantha Marah
```

This is a group assignment for the 4th year Networked Games module. The group consists of:
  * [Joe O'Regan](https://github.com/joeaoregan)
  * [Samantha Marah](https://github.com/jasfoley)
  * [Jason Foley](https://github.com/samanthamarah)

![alt text](https://raw.githubusercontent.com/joeaoregan/Yr4-NetworkGames-Hangman/master/Screenshots/8HangmanDualStackTCP.png "Dual Stack Client and Server")
Dual Stack Client and Server communicating over IPv4
![alt text](https://raw.githubusercontent.com/joeaoregan/Yr4-NetworkGames-Hangman/master/Screenshots/8HangmanDualStackTCPIPv6.png "Dual Stack Client and Server")
Dual Stack Client and Server communicating over IPv6

## 8-Hangman-Dual-Stack-TCP

A Hangman **TCP Client** and **Server** that can handle connections to and from `IPv4` and `IPv6` addresses

###### Server: 

**Dual Stack Server** can accept connections from Clients connecting with IPv4 or IPv6 sockets. 
Using `createDualStackServerSocket()` a `sockaddr_in6` structure is used to store an `IPv6` or `IPv4` address (formatted as IPv6)

###### Client:

The **Dual Stack Client**, can take an IPv4 or IPv6 command line parameter and create the appropriate socket, before connecting to the **Server**. 
The **Client** communicates with the **Server** using `read()` and `write()` if an `IPv4` address is supplied, 
`createClientSocketType()` uses a `sockaddr_in` address structure, for an `IPv6` address a `sockaddr_in6` address structure is created, 
`createTCPClientDualStack` uses the IP address parameter, and the port number and calls `getaddrinfo()` 
creating an `addrinfo` address structure from which the address family can be determined.

##### Files:

Code and application included in folder:
|  | File        | Description           |
| --- | ------------- |-------------|
| 1. | **serverds.c** | Dual Stack Server Code |
| 2. | **clientds.c** | Dual Stack Client Code |
| 3. | **srv** | Dual Stack Server Application |
| 4. | **cli** | Dual Stack Client Application |

##### Dependencies (Placed in root directory):
Files the Dual Stack Server is dependent on: 

1. DrawHangman.h
2. CreateTCPSocket.h
3. TCPPlayHangman.h

Additional files the **Dual Stack Client** is dependent on:

1. HandleErrors.h

## Running:

To run the included compiled **Dual Stack Server** with default port number `1066` (Different to UDP sockets`1068`):
```c
./srv
```

To run **Server** specifying the port number to listen for **Client** connections on e.g. port `8000`:
```c
./srv 8000
```

###### With the Dual Stack TCP Server application running, the Client must use the same port when sending data


The already compiled **Dual Stack Client** application included in the directory can be run with commands: 

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

To compile the **Dual Stack Server** code:
```c
gcc -o srv serverds.c
```

To compile the **Dual Stack Client** code:
```c
gcc -o cli clientds.c
```

