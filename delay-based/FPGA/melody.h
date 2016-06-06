#ifndef MELODY_H
#define MELODY_H

void PlayMelody();

// Define Melody

/* Mary had a little lamb
 #define melLength 26
int melody[melLength] = {12, 4, 12, 2, 12, 0, 12, 2, 12, 4, 12, 4, 24, 4, 12, 2,
12, 2, 24, 2, 12, 0, 12, 7, 24, 7};
 */

/* Dreamy*/
#define melLength 48

int melody[melLength] = {14, 0, 14, 4, 14, 14, 14, 11, 14, 14, 14, 4,
                         14, 0, 14, 4, 14, 14, 14, 11, 14, 14, 14, 4,
                         14, 5, 14, 9, 14, 12, 14, 16, 14, 12, 14, 9,
                         14, 5, 14, 9, 14, 12, 14, 16, 14, 12, 14, 9};

// Init Melody
int melCount = 0;
int melGain = 1;   // for melody note rest
int b = 0;         // index for melody arr current beat
int duration = 0;  // period between note change

void PlayMelody() {
  if (melCount == 0) {  // Change to next note
    melGain = 1;
    pitch_factor = pow(2, melody[b + 1] / 12.0);
    duration = melody[b];
    printf("Pitch Factor : %f, Semitone: %d, Duration : %d\n", pitch_factor,
           melody[b + 1], duration);
    b = (b + 2) % melLength;
  }
  if (melCount > duration * 0.9) {  // melody decay last 10%
    melGain = 0;
    printf("Melody Gain : %d\n", melGain);
  }

  melCount = (melCount + 1) % duration;
}

#endif /* MELODY_H */
