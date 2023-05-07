CC = clang -std=c99 -O2
DC = clang -std=c99 -O0 -g

default: cprd

cprd:
	${CC} -o cprd.out cprd.c

cprd_debug:
	${DC} -o cprd_debug.out cprd.c
