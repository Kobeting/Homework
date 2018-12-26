all: lab2

CXX = g++

OFLAGS = -pedantic -Wall -fomit-frame-pointer -funroll-all-loops -O3 -DNDEBUG 

LINKFLAGS = -static 

lab2: main.o floorplan.o parser.o
	$(CXX) $(LINKFLAGS) main.o floorplan.o parser.o -o lab2

main.o: main.cpp
	$(CXX) $(LINKFLAGS) main.cpp -c

floorplan.o: floorplan.cpp floorplan.h
	$(CXX) $(LINKFLAGS) floorplan.cpp -c

parser.o: parser.cpp parser.h
	$(CXX) $(LINKFLAGS) parser.cpp -c

clean:
	rm -rf *.o *.gch Lab2
