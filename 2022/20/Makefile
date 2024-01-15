PROGRAMS = part1 part2

CCFLAGS = -Wall -pedantic -std=c++17 -O3

all: $(PROGRAMS)

$(PROGRAMS): %: %.cpp
	g++ $(CCFLAGS) -O3 -o $@ $<

clean:
	rm -rf part1 part2
