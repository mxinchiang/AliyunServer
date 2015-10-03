all:server

OBJ=server.o se_socket.o

# Option for development
CFLAGS= -g

# Option for release
#CFLAGS= -o 
 
server:server.o se_socket.o
	gcc -Wall $(OBJ) $(CFLAGS) -o server  -l pthread
server.o:server.c se_socket.h
	gcc -Wall $(CFLAGS) -c $<
se_socket.o:se_socket.c se_socket.h
	gcc -Wall $(CFLAGS) -c $<

clean:
	rm *.o server
