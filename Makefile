FLAGS = -g -Wall
CC = gcc
.PHONY: clean

all: shell serv

shell: shell.o
	$(CC) $(FLAGS) -o shell shell.o

shell.o: shell.c
	$(CC) $(FLAGS) -c shell.c

serv: tcp_server.o
	$(CC) $(FLAGS) -o serv tcp_server.o

tcp_server.o: tcp_server.c
	$(CC) $(FLAGS) -c tcp_server.c

clean:
	rm -f *.o *.a *.so serv shell



