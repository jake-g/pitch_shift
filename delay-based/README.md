# Delay Based Pitch Shift
Click `Notes` to open up Google Doc Notes or click [HERE](https://docs.google.com/document/d/1LKrE6UvQ-VAJaBaG3KGaZTO_FA2HJm6xwDNvDgaG8XE/edit#)
### Matlab Demo
Based off the Notes
* navigate to  `Matlab Demo/`
* run `main.m`

### Linux C Demo
`PitchShift.h` Mimics the Matlab code and `pitchShiftWav.c` is nearly identical to the phase vocoder code. Requires `libsndfile`
* navigate to  `Linux/`
* Compile : run `make` (`make clean` is also available)
* Execute:  `./shiftWav semitones file.wav`
* Note: `semitones` is an int, `file.wav` must be in the `sound_files/` folder

### FPGA and GUI
Runs on FPGA in realtime and has Matlab GUI

See [README](https://github.com/jake-g/audio_fx/blob/master/delay-based/FPGA/README.md)
