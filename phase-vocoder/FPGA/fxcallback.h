#ifndef FX_CALLBACK_H
#define FX_CALLBACK_H

#include "PitchShift.h"

#define SAMPLING_RATE       (8000)
#define FRAMES_PER_BUFFER   (512)


// Current semitone difference from original input 
long semitones = 3;
// Number of samples in a single measure
int measure = 10000;
// Sample count in current measure
int sampleCount = 0;

// Pitch Shift variables
long buffer_size = FRAMES_PER_BUFFER;
//float sr = SAMPLING_RATE;
long fftSize = 256;
long osamp = 4;
// pitchShift variable defined in PitchShift function
//float pitchShift = pow(2., semitones/12.);

float inputBuffer[FRAMES_PER_BUFFER];
float outputBuffer[FRAMES_PER_BUFFER];

#endif /* FX_CALLBACK_H */
