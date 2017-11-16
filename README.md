# Year 4: Networked Games CA
# Hangman
## 16/11/2017

This is a group assignment for the 4th year Networked Games module. The group consists of:
  * Joe O'Regan
  * Samantha Marah
  * Jason Foley

## Features:

This online text based version of Hangman includes the following features:
  * TCP server socket using fork()
  * TCP server socket using select(), handles more than one client
  * UDP client and server playing for one game
  * Modular design with Hangman.h and DrawHangman.h files containing functions common to the different socket versions

Features yet to be implemented include:
  * A client that handles asynchronous data inputs from the network / server, and from the keyboard / user correctly
  * All server and clients handling a dual TCP/IP stack (IPv4 and IPv6). With the client handling naming and the DNS.
