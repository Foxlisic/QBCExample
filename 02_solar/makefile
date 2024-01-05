all:
	g++ main.cc -I.. -o main -lSDL2_mixer -lSDL2 -lm
	./main
ppm:
	ffmpeg -framerate 50 -r 50 -i out/record.ppm -sws_flags neighbor -sws_dither none -q:v 0 -y record.avi
