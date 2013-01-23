all: pcalc

pcalc: pcalc.o

pcalc.o: pcalc.h pcalc.c
	clang -c pcalc.c

clean:
	rm -rf *.o
	rm -rf *.gch

clobber: clean
	rm -rf pcalc

run:
	make clean
	make clobber
	make
	clear
	./pcalc rpn_files/publicCount.rpn