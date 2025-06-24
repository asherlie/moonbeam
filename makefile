CC=gcc
CFLAGS= -Wall -Wextra -Wpedantic -Werror -Wno-unused-result -lIL

all: img

img: img.c

clean:
	rm -f img
