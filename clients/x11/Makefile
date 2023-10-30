CC=gcc
CFLAGS= -O2 -Wall
INC= -I/usr/X11/include -I/usr/include/libxml2
LIB= -L/usr/X11/lib -L/usr/local/lib -lxml2
LIBS= -lX11 -lXpm

TARGET=ikafeclient

all: main.o
	$(CC) $(LIB) $(LIBS) -o $(TARGET) main.o

clean:
	rm -f $(TARGET)

install: all
	cp $(TARGET) /usr/local/bin/

main.o: main.c logo.xpm locked.xpm
	$(CC) $(CFLAGS) $(INC) -c main.c
