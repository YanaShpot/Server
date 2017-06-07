all: Server

Server: main.o
	g++ main.o -o Server

main.o: main.cpp
	g++ -c main.cpp

.PHONY : clean
clean:
	rm -rf *.o Server