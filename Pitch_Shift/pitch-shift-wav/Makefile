# EE 443 Final Project Makefile
# Jake Garrison, Jisoo Jung

execFileName: smbPitchShift.o snd2arr.o
	gcc -Wall -g -std=c11 -o pitchShift smbPitchShift.o snd2arr.o -lm -lsndfile
	echo "DONE"

smbPitchShift.o: smbPitchShift.c
	gcc -Wall -g -std=c11 -c smbPitchShift.c

snd2arr.o: pitchShiftWav.c
	gcc -Wall -g -std=c11 -c pitchShiftWav.c

clean:
	rm -f *.o pitchShift
