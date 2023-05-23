#include "st_reactor.h"
#include <signal.h>

#define PORT "9034" 
reactor_t* myReactor;
// Get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
// Return a listening socket
int get_listener_socket(void)
{
    int listener;     // Listening socket descriptor
    int yes=1;        // For setsockopt() SO_REUSEADDR, below
    int rv;
    struct addrinfo hints, *ai, *p;

    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }
    
    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) { 
            continue;
        }
        
        // Lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // If we got here, it means we didn't get bound
    if (p == NULL) {
        return -1;
    }

    freeaddrinfo(ai); // All done with this

    // Listen
    if (listen(listener, 10) == -1) {
        return -1;
    }

    return listener;
}
void sighandler(int sig)
{
   if(sig == SIGKILL || sig == SIGINT )
   {
    printf("\n");
    stopReactor(myReactor);
    if(myReactor == NULL)
        return;
    if(myReactor->flag)
    waitFor(myReactor);
    free(myReactor->handlers);
    free(myReactor->pfds);
    free(myReactor->remoteIP);
    free(myReactor);
    printf("Server closed\n");
    close()
    exit(0);
   }
}
void handleClient(int fd, void *arg){
    char buf[256];    // Buffer for client data
    reactor_t* reactor = (reactor_t*)arg;
    int nbytes = recv(fd, buf, sizeof buf, 0);
    if (nbytes <= 0) {
    // Got error or connection closed by client
        if (nbytes == 0)  // Connection closed
            printf("pollserver: socket %d hung up\n", fd);
        else 
            perror("recv");
        close(fd); // Bye!
        removeFd(reactor, fd);
        } 
        else {// We got some good data from a client
            printf ("Client %d: %s\n",fd,buf);
            for(int j = 0; j < reactor->fd_count; j++) { // Send to everyone!
                int dest_fd = reactor->pfds[j].fd;
                // Except the listener and ourselves
                if (dest_fd != reactor->listener && dest_fd != fd) {
                    if (send(dest_fd, buf, nbytes, 0) == -1) {
                        perror("send");
                    }
                }
            }
        }
} // END handle data from client

void handleNewClient(int fd, void *arg){
    printf("in handle new\n");
    reactor_t* reactor = (reactor_t*) arg;
    int listener = reactor->listener;     // Listening socket descriptor
    int newfd;        // Newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // Client address
    socklen_t addrlen;
    addrlen = sizeof remoteaddr;
    newfd = accept(listener,(struct sockaddr *)&remoteaddr,&addrlen);
    if (newfd == -1) {
        perror("accept eror");
    }
    else {
    addFd(reactor, newfd,handleClient);
    printf("pollserver: new connection from %s on socket %d\n",
        inet_ntop(remoteaddr.ss_family,get_in_addr((struct sockaddr*)&remoteaddr),
        reactor->remoteIP, INET6_ADDRSTRLEN),newfd);
    }
}

int main(void)
{
    printf("Start\n");
    int listener;
    myReactor = createReactor();
    signal(SIGINT, sighandler);

    listener = get_listener_socket();

    if (listener == -1) {
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }
    printf("listen...\n");

    addFd(myReactor, listener, handleNewClient);

    startReactor(myReactor);
  
    waitFor(myReactor);
    
    return 0;

}
  