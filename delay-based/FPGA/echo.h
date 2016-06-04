#ifndef ECHO_H
#define ECHO_H

#define MAX_DELAY 2000
#define MIN_DELAY 1000
#define DELAY_BUFFER_LENGTH 5000

// Decay rate
float gain = 0.6;

// Current delay
int sampleDelay = MIN_DELAY;

// The rate of changing delay in echo mode. The number of samples
// to increment/decrement in every key press.
short delayInc = 50;

// Contains samples played. New sample will be stored in
// an index specified by delayIndex.
alt_16 delayedBuffer[DELAY_BUFFER_LENGTH];

// delayIndex: incremented by 1 every sample. 
// wraps around to zero when it reaches 5000
int delayIndex = 0;

// switch to control pitch echo mode: 1 -> on, 0 -> off
short switch2Status = 0;

// switch to control echo mode: 1 -> on, 0 -> off
short switch3Status = 0;



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
int getEchoSample() {
	if (MAX_DELAY == 0) {
		return 0;
	}
	return gain * delayedBuffer[(DELAY_BUFFER_LENGTH + delayIndex - sampleDelay) % DELAY_BUFFER_LENGTH];
}
#endif /* ECHO_H */
