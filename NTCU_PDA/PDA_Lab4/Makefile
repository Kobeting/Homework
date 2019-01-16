all: lab4

CXX = g++

OFLAGS = -pedantic -Wall -fomit-frame-pointer -funroll-all-loops -O3 -DNDEBUG 

LINKFLAGS = -static 

lab4: main.o parser.o
	$(CXX) $(LINKFLAGS) main.o parser.o -o lab4

main.o: main.cpp
	$(CXX) $(LINKFLAGS) main.cpp -c

parser.o: parser.cpp parser.h
	$(CXX) $(LINKFLAGS) parser.cpp -c

clean:
	rm -rf *.o *.gch Lab4
