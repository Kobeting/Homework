all: Lab3

CXX = g++

OFLAGS = -Wno-long-long -pedantic -Wall -fomit-frame-pointer -funroll-all-loops -O3

LINKFLAGS = -static

Lab3: main.o rplParser.o message.o legalize.o
	$(CXX) $(OFLAGS) main.o rplParser.o message.o legalize.o -o Lab3

main.o: main.cpp
	$(CXX) $(OFLAGS) -DCOMPILETIME="\"`date`\"" main.cpp -c

message.o: message.cpp message.h
	$(CXX) $(OFLAGS) message.cpp -c

rplParser.o: rplParser.cpp rplParser.h
	$(CXX) $(OFLAGS) rplParser.cpp -c

legalize.o: legalize.cpp legalize.h
	$(CXX) $(OFLAGS) legalize.cpp -c

clean:
	rm -rf *.o *.gch Lab3 dumpParser.txt
