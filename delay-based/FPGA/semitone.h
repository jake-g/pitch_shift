#ifndef SEMITONE_H
#define SEMITONE_H

// Number of total pitches is 25: 
// -12 semitones, +12 semitones, 0 semitone change
#define NUM_PITCH 25

// semitone factors ranging from -12 to 12 (+- 1 octave)
// In delay based pitch shifting algorithm, pitch shift rate is 
// computed as 2^(semitone/12) / Fs
float semitoneFactors[NUM_PITCH] = {
    0.0000625,
    0.0000662,
    0.0000702,
    0.0000743,
    0.0000787,
    0.0000834,
    0.0000884,
    0.0000936,
    0.0000992,
    0.0001051,
    0.0001114,
    0.0001180,
    0.0001250,
    0.0001324,
    0.0001403,
    0.0001487,
    0.0001575,
    0.0001669,
    0.0001768,
    0.0001873,
    0.0001984,
    0.0002102,
    0.0002227,
    0.0002360,
    0.0002500   
};

#endif /* SEMITONE_H */
