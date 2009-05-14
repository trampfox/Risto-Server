PROGS = error.o my_io.o my_signal.o wrapper.o function.o trisClient trisServer 
CC = gcc -g
CLEANFILES = error.o my_io.o my_signal.o wrapper.o function.o trisServer.o trisClient.o


all: ${PROGS}

error.o: lib/error.c
	${CC} -c lib/error.c

my_io.o:  lib/my_io.c
	${CC} -c lib/my_io.c

my_signal.o:  lib/my_signal.c
	${CC} -c lib/my_signal.c
	
wrapper.o: lib/wrapper.c
	${CC} -c lib/wrapper.c

function.o: function.c
	${CC} -c function.c

trisClient: trisClient.o error.o my_io.o wrapper.o function.o
	${CC} -o $@ $^

trisServer: trisServer.o my_signal.o error.o my_io.o wrapper.o function.o
	${CC} -o $@ $^
	
clean:
	rm -f ${CLEANFILES}
	rm -f ${PROGS}
	rm -f *.*~	
	rm -f *~
