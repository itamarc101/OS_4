#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "st_reactor.h"

void *createReactor()
{

    reactor_t *reactor = malloc(sizeof(reactor_t));
    reactor->fd_count = 0;
    reactor->fd_size = 5;
    reactor->pfds = malloc(sizeof(*reactor->pfds) * reactor->fd_size);
    reactor->remoteIP = malloc(INET6_ADDRSTRLEN);
    reactor->handlers = malloc(sizeof(*reactor->handlers) * reactor->fd_size);
    reactor->flag = 0;
    reactor->listener = 0;
    return reactor;
}
void *handleThreads(void *arg)
{
    reactor_t *reactor = (reactor_t *)arg;
    while (reactor->flag == 1)
    {
        int poll_count = poll(reactor->pfds, reactor->fd_count, -1);
        if (poll_count == -1)
        {
            perror("poll");
            exit(1);
        }
        for (int i = 0; i < reactor->fd_count; i++)
        {

            // Check if someone's ready to read
            if (reactor->pfds[i].revents & POLLIN)
            { // We got one!!

                if (reactor->pfds[i].fd == reactor->listener)
                {
                    reactor->handlers[i](reactor->pfds[i].fd, reactor);
                    break;
                }
                else
                {
                    reactor->handlers[i](reactor->pfds[i].fd, reactor);
                }
            }
        }
    }
    return NULL;
}
void startReactor(void *this)
{
    reactor_t *reactor = (reactor_t *)this;

    if (reactor == NULL || reactor->flag == 1)
        return;
    reactor->flag = 1;
    int num = pthread_create(&reactor->thread, NULL, handleThreads, reactor);
    if (num != 0)
    {
        perror("thread create");
        exit(0);
    }
}
void stopReactor(void *this)
{

    reactor_t *reactor = (reactor_t *)this;
    if (reactor == NULL || reactor->flag == 0)
        return;
    reactor->flag = 0;
    pthread_cancel(reactor->thread);
    pthread_join(reactor->thread, NULL);
    printf("Reactor stopped\n");
}
void addFd(void *this, int fd, handler_t handler)
{

    reactor_t *reactor = (reactor_t *)this;
    if(reactor->listener == 0)
        reactor->listener = fd;
    if (reactor->fd_count == reactor->fd_size)
    {
        reactor->fd_size *= 2; // Double it
        reactor->pfds = realloc(reactor->pfds, sizeof(*reactor->pfds) * (reactor->fd_size));
        reactor->handlers = realloc(reactor->handlers, sizeof(handler_t) * reactor->fd_size);
    }
    reactor->handlers[reactor->fd_count] = handler;
    reactor->pfds[reactor->fd_count].fd = fd;
    reactor->pfds[reactor->fd_count].events = POLLIN; // Check ready-to-read

    reactor->fd_count++;
}
void waitFor(void *this)
{
    reactor_t *reactor = (reactor_t *)this;
    if (reactor == NULL)
        return;
    if (reactor->flag)
    {
        int num = pthread_join(reactor->thread, NULL);
        if (num != 0)
        {
            perror("wait for");
            exit(0);
        }
    }
}
void removeFd(void *this, int fd)
{
    reactor_t *reactor = (reactor_t *)this;

    if (reactor == NULL || fd < 0 || fd >= reactor->fd_count)
    {
        return;
    }
    reactor->pfds[fd] = reactor->pfds[reactor->fd_count - 1];
    reactor->handlers[fd] = reactor->handlers[reactor->fd_count - 1];
    reactor->handlers[reactor->fd_count - 1] = NULL;
    reactor->fd_count--;
}
