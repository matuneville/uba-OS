CFLAGS=-g -Wall

.PHONY: all clean perf

all: ejercicio_3

%: %.c

clean:
	rm -f ejercicio_3

N ?= 1
perf: ejercicio_3
	perf stat -r 5 ./ejercicio_3 ${N}
