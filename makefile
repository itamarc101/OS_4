CC = gcc
WALL = -Wall -Werror -fpic
LFLAGS = -shared
LIBRARY = st_reactor.so
TARGET = react_server
SRCS = pollserver.c st_reactor.c
OBJS = $(SRCS:.c=.o)

all: $(LIBRARY) $(TARGET)

$(LIBRARY): st_reactor.o
	$(CC) $(LFLAGS) -o $@ $<

$(TARGET): $(OBJS)
	$(CC) $(WALL) -o $@ $(OBJS) -L. -l:$(LIBRARY)

%.o: %.c
	$(CC) $(WALL) -c $< -o $@

clean:
	rm -f $(OBJS) $(LIBRARY) $(TARGET)