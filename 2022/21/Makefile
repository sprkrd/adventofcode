PROGRAMS = part1 part2

all: $(PROGRAMS)

part1: part1.cpp
	g++ -Wall -Wno-sign-compare -pedantic -std=c++17 -O3 -o part2 part2.cpp

part2: part2.cpp
	g++ -Wall -Wno-sign-compare -pedantic -std=c++17 -O3 -o part2 part2.cpp -lz3

clean:
	rm -rf part1 part2
