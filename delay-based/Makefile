# EE 443 Final Project Makefile
# Jake Garrison, Jisoo Jung

# all: shiftWav shiftRealtime

shiftWav: pitchShiftWav.c
	gcc -Wall -g -std=c11 -o shiftWav pitchShiftWav.c -lm -lsndfile


# shiftRealtime: realtime.c
# 	gcc realtime.c -o shiftRealtime -lportaudio


clean:
	rm -f *.o shiftWav
	# rm -f *.o shiftRealtime
