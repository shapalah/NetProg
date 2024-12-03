CC = g++
CFLAGS = -Wall -Wextra -pedantic -std=c++17

client: client.cpp
 $(CC) $(CFLAGS) client.cpp -o client

server: server.cpp
 $(CC) $(CFLAGS) server.cpp -o server

all: client server

clean:
 rm -f client server
