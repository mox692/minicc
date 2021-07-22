CFLAGS=-std=c11 -g -fno-common
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

minicc: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJS): minicc.h

test: minicc
	./test.sh

.PHONY: debug 
debug:
	cc -std=c11 -g -O0 -fno-common   -c -o old_minicc.o old_minicc.c
	cc -std=c11 -g -O0 -fno-common   -c -o tokenize.o tokenize.c
	cc -std=c11 -g -O0 -fno-common   -c -o parser.o parser.c
	cc -std=c11 -g -O0 -fno-common   -c -o main.o main.c
	cc -std=c11 -g -O0 -fno-common   -c -o codegen.o codegen.c
	cc -std=c11 -g -O0 -fno-common -o minicc old_minicc.o tokenize.o parser.o main.o codegen.o