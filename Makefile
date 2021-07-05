CFLAGS=-std=c11 -g -static

minicc: minicc.c

test: minicc
	./test.sh

clean:
	rm -f 9cc *.o *~ tmp*

.PHONY: test clean