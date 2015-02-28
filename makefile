GCC = gcc -g

all: rc6

rc6: rc6.o
	$(GCC) rc6.o -o rc6

rc6.o: rc6.cpp
	$(GCC) -c rc6.cpp

clean:
	rm -f *.o *~core rc6
