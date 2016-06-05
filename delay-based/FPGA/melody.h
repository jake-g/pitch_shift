#ifndef MELODY_H
#define MELODY_H

#define melLength 25

// semitones to play
int melody[melLength] = {4,2,0,2,4,4,4,2,2,2,4,4,4,4,2,0,2,4,4,4,2,2,4,2,0}; 

//	#define melLength 8
//	int melody[melLength] = {0, 4, 7, 12, 4, 0, -7, -4}; // semitones to play

int b = 0;  // index for melody arr current beat
int melodyLoop = 10; // period between note change
int lp = 0; // loop counter

#endif /* MELODY_H */
