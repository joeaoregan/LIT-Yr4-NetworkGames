```c
   _____________          T E A M 1 -  H A N G M A N
   |/      |        Year 4: Networked Games CA - Hangman
   |   ___(")___                  26/11/2017			    Original
   |      |_| 							Hangman Listings
   |      /^\                    Joe O'Regan
   |    _// \\_                  Jason Foley
 __|___________                 Samantha Marah
```

This is a group assignment for the 4th year Networked Games module. The group consists of:
  * [Joe O'Regan](https://github.com/joeaoregan)
  * [Samantha Marah](https://github.com/jasfoley)
  * [Jason Foley](https://github.com/samanthamarah)

![alt text](https://raw.githubusercontent.com/joeaoregan/Yr4-NetworkGames-Hangman/master/Screenshots/1OriginalHangmanListings.png "Original Hangman Client and Server")
###### Original Hangman Game
![alt text](https://raw.githubusercontent.com/joeaoregan/Yr4-NetworkGames-Hangman/master/Screenshots/WireShark/1WiresharkCapture.png "Original Hangman Client and Server Communication")
###### Original Hangman Client and Server communication

## 1-Original-Hangman-Listings

The Original Hangman code, unmodified with errors. Using `read()` and `write()` to communicate

##### Files:

Code and application included in folder:

No. | File | Description 
--- | --- | ---
1 | **hangserver_iter.c** | Original Server Code (Unchanged)
2 | **hangclient.c** | Original Client Code (Unchanged)
3 | **words** | List of words to randomly from

## Running:
To run the existing compiled **Server**:
```c
./srv
```
To run the existing compiled **Client**: 
```c
./cli 127.0.0.1
```
The words file must be in the same directory

## Compiling:
To compile the **Client** code:
```c
gcc -o cli hangclient.c
```
To compile the **Server** code:
```c
gcc -o srv hangserver_iter.c
```
Where `cli` and `srv` are the names of the compiled programs
