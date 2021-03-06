PROGS=server client
CC=gcc
CFLAGS=-std=gnu99 -ggdb
SRCSVR=server.c
SRCCLT=client.c

all: ${PROGS}

server: ${SRCSVR}
	${CC} ${CFLAGS} ${SRCSVR} -o server
	
client: ${SRCCLT}
	${CC} ${CFLAGS} ${SRCCLT} -o client

clean: 
	rm ${PROGS}

.PHONY: clean
.PHONY: all
