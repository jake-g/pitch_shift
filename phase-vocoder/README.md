# Usage
#### Jake Garrison, Jisoo Jung

## File structure
  * Pitch_Shift folder:
    * pitchShift.h
    * pitchShiftWav.c
    * realtime.c
    * sound_files:
    	* all sound files

## How to build
  * make clean
  * make

## Process Wav File
#### Run
  * Command: `./shiftWav semitones inputFilename_with_extension`
  * semitones is an integer with negative being shift to lower pitch and positive being shit to higher pitch
  * e.g. `./shiftWav -3 Maple.wav`

#### Output file name
  * Uses pattern input-out.wav
  * e.g. If input file name is `Maple.wav` output file name is `Maple-out.wav`

## Process Realtime
#### Run
  * Command: `./shiftRealtime` (no input args yet...)
  * Note, uses port audio drivers to read mic and and output to os
