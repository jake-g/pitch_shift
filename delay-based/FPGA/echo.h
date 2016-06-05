#ifndef ECHO_H
#define ECHO_H

#define MAX_DELAY 2000
#define MIN_DELAY 1000
#define DELAY_BUFFER_LENGTH 5000

#define MAX_PITCH 4 // two octaves up
#define MIN_PITCH 0
#define MAX_PITCH_RATE (0.3)
#define MIN_PITCH_RATE (-0.3)

// Decay rate
float gain = 0.6;

// Current delay
int sampleDelay = MIN_DELAY;

// Current pitch factor
float pitch_factor = 1;

// Current pitch change rate
float pitchChangeRate = 0.1;

// The rate of changing delay in echo mode. The number of samples
// to increment/decrement in every key press.
short delayInc = 50;

// Contains samples played. New sample will be stored in
// an index specified by delayIndex.
alt_16 delayedBuffer[DELAY_BUFFER_LENGTH];

// delayIndex: incremented by 1 every sample. 
// wraps around to zero when it reaches 5000
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

// returns echo sample
int getEchoSample() {
	if (sampleDelay == 0) {
		return 0;
	}
	return gain * delayedBuffer[(DELAY_BUFFER_LENGTH + delayIndex - sampleDelay) % DELAY_BUFFER_LENGTH];
}

// Manages pitchChangeRate. Called from key interrupts.
// Changes pitch rate by 0.1
// How rate of pitch change changes. 
void changePitchRate(short sign) {
	pitchChangeRate = pitchChangeRate + sign * 0.1;
	if (pitchChangeRate < MIN_PITCH_RATE) {
		pitchChangeRate = MIN_PITCH_RATE;
	} else if (pitchChangeRate > MAX_PITCH_RATE) {
		pitchChangeRate = MAX_PITCH_RATE;
	}
	printf("pitchChangeRate = %f\n", pitchChangeRate);
}

// Manages pitch_factor. Called after every 5000 sample had been processed.
void changePitch() {
	// in pitch_factor range [0, 0.1], increment/decrement is 0.01
	if (pitch_factor <= 0.1) {
		pitch_factor = pitch_factor + pitchChangeRate / 10;
	} else {
		pitch_factor = pitch_factor + pitchChangeRate;
	}
	// wrap around pitch_factor from MIN to MAX, MAX to MIN
	if (pitch_factor < MIN_PITCH) {
		pitch_factor = MAX_PITCH;
	} else if (pitch_factor > MAX_PITCH) {
		pitch_factor = MIN_PITCH;
	}
}
#endif /* ECHO_H */
