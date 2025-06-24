CC=gcc
CFLAGS= -Wall -Wextra -Wpedantic -Werror -Wno-unused-result -lIL -g3

all: img

img: img.c

clean:
	rm -f img
