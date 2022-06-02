CFLAGS ?= -O2 -g

CFLAGS += -std=gnu99

CFLAGS += -Wall -Werror -Wformat-security -Wignored-qualifiers -Winit-self \
	-Wswitch-default -Wpointer-arith -Wtype-limits -Wempty-body \
	-Wstrict-prototypes -Wold-style-declaration -Wold-style-definition \
	-Wmissing-parameter-type -Wmissing-field-initializers -Wnested-externs \
	-Wstack-usage=4096 -Wmissing-prototypes -Wfloat-equal -Wabsolute-value

CFLAGS += -fsanitize=undefined -fsanitize-undefined-trap-on-error

CC += -m32 -no-pie -fno-pie

.PHONY: all test clean

all: integral

main.o: main.c
		$(CC) $(CFLAGS) -c main.c

funcs.o: funcs.asm
		nasm -f elf32 funcs.asm

integral: main.o funcs.o
		$(CC) -o integral main.o funcs.o

test: integral
		./integral --test-integral 1:-2.0:2.0:0.00014:15.25372
		./integral --test-integral 1:0:2.718281:0.00007:19.5908
		./integral --test-integral 1:-11:-10.0:22:2.00002
		./integral --test-integral 2:-30.0:-29:0.841:0.0339
		./integral --test-integral 2:-2.6:-2.4:0.0058:0.08004
		./integral --test-integral 2:1.0:2.0:0.001:-0.693
		./integral --test-integral 3:-10:-1.0:0.0015:27.00000
		./integral --test-integral 3:-1:2.0:0.0015:-3.0000
		./integral --test-integral 3:-2:0:0.0015:0.0000
		./integral --test-root 1:2:-1:-0.1:0.00001:-0.3718
		./integral --test-root 1:2:-0.4:-0.3:0.0001:-0.3718
		./integral --test-root 1:2:-0.4:-0.37:0.0001:-0.3718
		./integral --test-root 1:3:-4.5:-3.9:0.0015:-4.0267
		./integral --test-root 1:3:-4.5:-3:0.0014:-4.0267
		./integral --test-root 1:3:-4.5:0:0.0015:-4.0267
		./integral --test-root 2:3:-2.1:-1.5:0.0009:-1.82279
		./integral --test-root 2:3:0.1:1.1:0.00001:0.82287
		./integral --test-root 2:3:0.8:1.5:0.00045:0.82287

clean:
		rm integral *.o
