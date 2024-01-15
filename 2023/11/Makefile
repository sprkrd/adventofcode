PROGRAMS = part1 part2

all: $(PROGRAMS)

$(PROGRAMS): %: %.cpp
	g++ -Wno-sign-compare -Wall -pedantic -std=c++17 -O3 -o $@ $<

clean:
	rm -rf part1 part2
