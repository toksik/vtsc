CC=gcc

all: evlogger

clean:
	rm -f evlogger
	rm -f *.o

evlogger: evdev.o evlogger.o
	${CC} evdev.o evlogger.o -o evlogger

evdev.o: evdev.c evdev.h
	${CC} -c evdev.c -o evdev.o

evlogger.o: evlogger.c evdev.h
	${CC} -c evlogger.c -o evlogger.o
