CC = arm-linux-gnueabihf-gcc
CC2 = gcc
CFLAGS = -Wall -g -std=c99 -D_POSIX_C_SOURCE=200809L -Werror -pthread

# List of source files
SRC = board.c config.c pinMap.c main.c

# List of header files
HDR = board.h config.h pinMap.h

# Output executable
TARGET = chess

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)
#cp $(TARGET) $(HOME)/cmpt433/public/myApps/

this:
	$(CC2) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

