include GNUmakevars
include GNUmakerules
CFLAGS= -g $(INCLUDES)


all: server client

clean:
	rm -f *.bak *.o server core client

distclean: clean
	rm -f *.d

client: client.o gethostbynameornumber.o
server: server.o gethostbynameornumber.o

-include *.d
