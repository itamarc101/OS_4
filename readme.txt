Itamar Cohen 209133826
Naor Ladani 318664190

Operating System Course Assignment 4
Computer Science Ariel University

Description about the assignment:
This project is a chat that supports unlimited amount to clients while using poll and reactor.

Reactor library has some functinos which are:
A) void* createReactor 
 • Creates a Reactor, returns a pointer to the Reactor structure that will be redirected to the following functions.
When the Reactor is created he doesnt work but some of the data structures will be init'd and allocated

B) void stopReactor
 • stops the Reactor if it's on. otherwise does nothing

C) void startReactor
 • Starts a thread of Reactor. The thread will be in a busy loop and will call select or poll. (in our project will be poll)

D) void addFd
 • When the handler_t is a pointer to a function that will call when the fd is 'hot'.
   which means add the fd to the Reactor.

E) void WaitFor 
 • Waits untill the Reactor thread will finish and then join it.
 
F) void* handleThreads
 • Handles threads check if there is a process that has available information and predicts   
   between processes.
H) void removeFD
 • responsible of removing specific file descriptor from the Reactor.

pollserver - During the process of the project we used pollserver file from slide of lesson 5 which found in the moodle.
we predicted the functions and divided it to handlers functions.
we used signal to use SIGKILL SIGINT to catch errors from the server side or it's being terminated.

HOW TO RUN??
make all - creates all the server files
gcc -o client client.c

creation of a client
client 127.0.0.1
 
   


   

