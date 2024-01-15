PROGRAMS = part1

all: $(PROGRAMS)

$(PROGRAMS): %: %.cpp
	g++ -Wall -Wno-sign-compare -pedantic -std=c++17 -O3 -o $@ $<

clean:
	rm -rf part1 part2
