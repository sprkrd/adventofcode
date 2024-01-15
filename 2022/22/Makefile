PROGRAMS = part1 part2

CCFLAGS = -Wall -pedantic -std=c++17 -O3
CCFLAGS_D = -Wall -pedantic -std=c++17 -O0 -g

all: $(PROGRAMS)

$(PROGRAMS): %: %.cpp
	g++ $(CCFLAGS) -o $@ $<

clean:
	rm -rf part1 part2
