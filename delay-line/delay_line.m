input_file = 'sine-wave.wav';
sign = 1; % 1 is up 0 is down;
semitones = -4;

output = pitch_shift(input_file, semitones);


[input, Fs]= audioread(input_file);
% output = audioread('../sound_files/Maple_short-out_(5).wav');
showFFT(input, output, Fs)

soundsc(input, Fs)
pause()
soundsc(output, Fs)