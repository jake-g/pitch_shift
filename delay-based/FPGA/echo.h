#ifndef ECHO_H
#define ECHO_H

#include "PitchShift.h"

#define MAX_DELAY 1000
#define MIN_DELAY 0
#define BUFFER_LENGTH 4000

float gain = 0.6;
int sampleDelay = 0;
short delayInc = 50;
int delayedBuffer[BUFFER_LENGTH];
// delayIndex: incremented by 1 every sample. 
// wraps around to zero when it reaches 4000
int delayIndex = 0; 

// usage: e.g. call every time when the button is pressed
// param: sign is either -1 or 1
void changeDelay(short sign) {
	sampleDelay = sampleDelay + sign * delayInc;
	if (sampleDelay < MIN_DELAY) {
		sampleDelay = MIN_DELAY;
	} else if (sampleDelay > MAX_DELAY) {
		sampleDelay = MAX_DELAY;
	}
	printf("sampleDelay = %d\n", sampleDelay);
}

// called 
static int getEchoSample() {
	if (sampleDelay == 0) {
		return 0;
	}
	return gain * delayedBuffer[(BUFFER_LENGTH + delayIndex - sampleDelay) % BUFFER_LENGTH];
}

#endif /* ECHO_H */