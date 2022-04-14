FLAGS = -g -Wall
CC = gcc
.PHONY: clean

all: shell server

shell: main.o
	$(CC) $(FLAGS) -o shell main.o

main.o: main.c
	$(CC) $(FLAGS) -c main.c

server: server.o
	$(CC) $(FLAGS) -o server server.o

tcp_server.o: tcp_server.c
	$(CC) $(FLAGS) -c server.c

clean:
	rm -f *.o *.a *.so server shell



