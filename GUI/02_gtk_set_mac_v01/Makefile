OBJS = main.o
CFLAGS = -g -Wall `pkg-config --cflags gtk+-2.0`
LDFLAGS = `pkg-config --libs gtk+-2.0`
CC = gcc
RM = rm
TARGET = main

all: $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

clean:
	$(RM) $(OBJS) $(TARGET)
