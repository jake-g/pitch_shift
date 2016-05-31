%% Pitch Shift
input_file = 'sine-wave.wav';
semitones = 4;
output = pitch_shift(input_file, semitones);

%% See Results
[input, Fs]= audioread(input_file);
showFFT(input, output, Fs)

%% Play Results (press key to end pause)
soundsc(input, Fs)
pause()
soundsc(output, Fs)