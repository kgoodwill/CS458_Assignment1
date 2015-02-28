GPP = g++ -g -std=c++11

all: rc6

rc6: rc6.o
	$(GPP) rc6.o -o rc6

rc6.o: rc6.cpp
	$(GPP) -c rc6.cpp

clean:
	rm -f *.o *~core rc6
