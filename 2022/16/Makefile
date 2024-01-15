PROGRAMS = part1 part2

CCFLAGS = -Wall -pedantic -std=c++17 -O3

all: $(PROGRAMS)

utils.o: utils.hpp utils.cpp
	g++ $(CCFLAGS) -c utils.cpp

$(PROGRAMS): %: %.cpp utils.o
	g++ $(CCFLAGS) -O3 -o $@ $< utils.o

clean:
	rm -rf part1 part2
