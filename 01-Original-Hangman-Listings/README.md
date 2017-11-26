```c
   _____________          T E A M 1 -  H A N G M A N
   |/      |        Year 4: Networked Games CA - Hangman
   |   ___(")___                  26/11/2017
   |      |_| 
   |      /^\                    Joe O'Regan
   |    _// \\_                  Jason Foley
 __|___________                 Samantha Marah
```

This is a group assignment for the 4th year Networked Games module. The group consists of:
  * [Joe O'Regan](https://github.com/joeaoregan)
  * [Samantha Marah](https://github.com/jasfoley)
  * [Jason Foley](https://github.com/samanthamarah)

## 1-Original-Hangman-Listings

The Original Hangman code, unmodified with errors. Using `read()` and `write()` to communicate

##### Files:
1. hangclient.c
2. hangserver_iter.c
3. words

## Running:
To run the existing compiled Server:
```c
./srv
```
To run the existing compiled client: 
```c
./cli 127.0.0.1
```
## Compiling:
To compile the client code:
```c
gcc -o cli hangclient.c
```
To compile the server code:
```c
gcc -o srv hangserver_iter.c
```
Where `cli` and `srv` are the names of the compiled programs
