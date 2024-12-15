PROGRAMS = part1 part2

all: $(PROGRAMS)

part1: part1.cpp
	g++ -Wall -pedantic -std=c++17 -O3 -o part1 part1.cpp

part2: part2.cpp
	g++ -Wall -pedantic -std=c++17 -O3 -o part2 part2.cpp -lncurses -ltinfo

clean:
	rm -rf part1 part2
