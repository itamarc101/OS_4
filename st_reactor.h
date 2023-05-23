#ifndef ST_REACTOR_H
#define ST_REACTOR_H
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>

typedef void (*handler_t)(int, void* arg);

typedef struct {
    pthread_t thread;           
    handler_t *handlers; 
    struct pollfd* pfds;        
    char *remoteIP;              
    int flag;              
    int listener;              
    int fd_count;             
    int fd_size;               
} reactor_t;

void* handleThreads(void* arg);
void stopReactor(void * this);
void startReactor(void* this);
void addFd(void* this, int fd, handler_t handler);
void waitFor(void* this);
void* createReactor();
void removeFd(void* this, int fd);
void *get_in_addr(struct sockaddr *sa);
int get_listener_socket(void);
void handleNewClient(int fd, void *arg);

#endif