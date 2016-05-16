# EE 443 Final Project Makefile
# Jake Garrison, Jisoo Jung

pitchShift: pitchShiftWav.o
	gcc -Wall -g -std=c11 -o pitchShift pitchShiftWav.o -lm -lsndfile
	echo "DONE"

pitchShiftWav.o: pitchShiftWav.c
	gcc -Wall -g -std=c11 -c pitchShiftWav.c

clean:
	rm -f *.o pitchShift