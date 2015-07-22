CC = c99
CFLAGS = -Wall -pedantic
LDLIBS = -lpthread

all: AirportSimulator

AirportSimulator: AirportSimulator.o AirportSimLib.o

AirportSimulator.o: AirportSimulator.c

AirportSimLib.o: AirportSimLib.c

clean:
    rm -f *.o
