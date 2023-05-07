GEN = clang -std=c99 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700
CC = ${GEN} -O2
DC = ${GEN} -O0 -g

default: cprd

cprd:
	${CC} -o cprd.out cprd.c

cprd_debug:
	${DC} -o cprd_debug.out cprd.c
