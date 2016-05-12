# How to use pitchShiftWav.c - Thursday, May 12

## Jake Garrison, Jisoo Jung

## File structure
  pitch-shift-wav folder:
    pitchShiftWav.c
    pitchShift.c
    sound_files:
      all sound files

## How to build
  make clean
  make

## How to run
  Format: ./pitchShift semitones inputFilename_with_extension
  semitones is an integer with negative being shift to lower pitch and positive being shit to higher pitch
  e.g. ./pitchShift -3 Maple.wav

## Output file name
  input-out.wav
  e.g. If input file name is "Maple.wav" output file name is "Maple-out.wav"
