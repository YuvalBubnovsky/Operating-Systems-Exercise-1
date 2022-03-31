FLAGS = -g -Wall
CC = gcc
.PHONY: clean

all: shell server

shell: shell.o
	$(CC) $(FLAGS) -o shell shell.o

shell.o: shell.c
	$(CC) $(FLAGS) -c shell.c

server: server.o
	$(CC) $(FLAGS) -o server server.o

tcp_server.o: tcp_server.c
	$(CC) $(FLAGS) -c server.c

clean:
	rm -f *.o *.a *.so server shell



