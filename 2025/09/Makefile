PROGRAMS = part1 part2

all: $(PROGRAMS)

$(PROGRAMS): %: %.cpp
	g++ -Wall -pedantic -std=c++17 -O3 -o $@ $<

clean:
	rm -rf part1 part2

video.mp4: part2
	mkdir -p out
	./part2 out/frame_ < input
	ffmpeg -framerate 24 -i out/frame_%03d.ppm -c:v libx264 -pix_fmt yuv420p video.mp4

animation.gif: video.mp4
	ffmpeg -framerate 24 -i out/frame_%03d.ppm -vf palettegen palette.png
	ffmpeg -framerate 24 -i out/frame_%03d.ppm -i palette.png -filter_complex "paletteuse" animation.gif
