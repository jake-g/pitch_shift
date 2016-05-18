# EE 443 Final Project Makefile
# Jake Garrison, Jisoo Jung

all: shiftWav shiftRealtime


shiftWav: pitchShiftWav.c
	gcc -Wall -g -std=c99 -o shiftWav pitchShiftWav.c -lm -lsndfile


shiftRealtime: realtime.c
	gcc -Wall -g -std=c99 -o shiftRealtime realtime.c -lportaudio


clean:
	rm -f *.o shiftWav
	rm -f *.o shiftRealtime
