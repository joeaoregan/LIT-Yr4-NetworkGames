#     T E A M 1
#   H A N G M A N
#   _____________
#   |/      |
#   |   ___(")___
#   |      |_|
#   |      /^\
#   |    _// \\_
# __|____________
# 
# Year 4: Networked Games CA
# Hangman
## 25/11/2017

This is a group assignment for the 4th year Networked Games module. The group consists of:
  * [Joe O'Regan](https://github.com/joeaoregan)
  * [Samantha Marah](https://github.com/jasfoley)
  * [Jason Foley](https://github.com/samanthamarah)

## Features:

This online text based version of Hangman includes the following features:
  * TCP server socket using fork()
  * TCP server socket using select(), handles more than one client
  * UDP client and server playing for one game
  * Modular design with Hangman.h and DrawHangman.h files containing functions common to the different socket versions
  * A client that handles asynchronous data inputs from the network / server, and from the keyboard / user correctly

Features yet to be implemented include:
  * All server and clients handling a dual TCP/IP stack (IPv4 and IPv6). With the client handling naming and the DNS.
    (The server can handle connections from both IPv4 and IPv6 clients, but the clients need to be separate programs)
