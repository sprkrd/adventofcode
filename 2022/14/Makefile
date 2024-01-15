PROGRAMS = part1 part2

all: $(PROGRAMS)

$(PROGRAMS): %: %.cpp
	g++ -Wall -Wno-sign-compare -pedantic -std=c++17 -O3 -o $@ $< -lncurses

clean:
	rm -rf part1 part2
