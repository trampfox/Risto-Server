PROGS = error.o my_io.o my_signal.o helloClient helloServer 
CC = gcc -g -Wall
CLEANFILES = error.o my_io.o my_signal.o helloServer.o helloClient.o


all: ${PROGS}

error.o: error.c
	${CC} -c error.c

my_io.o:  my_io.c
	${CC} -c my_io.c

my_signal.o:  my_signal.c
	${CC} -c my_signal.c
	

helloClient: helloClient.o error.o my_io.o
	${CC} -o $@ $^

helloServer: helloServer.o my_signal.o error.o my_io.o
	${CC} -o $@ $^
	
clean:
	rm -f ${CLEANFILES}
	rm -f ${PROGS}
	rm -f *.*~	
	rm -f *~
	clear