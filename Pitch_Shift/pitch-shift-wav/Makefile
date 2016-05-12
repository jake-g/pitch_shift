# EE 443 Final Project Makefile
# Jake Garrison, Jisoo Jung

pitchShift: PitchShift.o pitchShiftWav.o
	gcc -Wall -g -std=c11 -o pitchShift PitchShift.o pitchShiftWav.o -lm -lsndfile
	echo "DONE"

PitchShift.o: PitchShift.c
	gcc -Wall -g -std=c11 -c PitchShift.c

pitchShiftWav.o: pitchShiftWav.c
	gcc -Wall -g -std=c11 -c pitchShiftWav.c

clean:
	rm -f *.o pitchShift