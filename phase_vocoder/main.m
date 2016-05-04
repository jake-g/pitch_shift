% Input
input = 'speech_files/Maple.wav' % piano 
% input = 'speech_files/vowels_10Hz_edited.wav'
% input = 'speech_files/vowels_300Hz_edited.wav'

speed_factor = 0.3;  % slow down
fft_size = 1024;

%% Change Speed
[x,sr] = audioread(input);
y = pvoc(x, speed_factor); % pvoc(input, factor)
len = max(length(y), length(x)); % longer signal

% Play 
% sound(x, sr) 
% pause(); % press any key
% sound(y, sr)

% Plot
figure
subplot(2,1,1); plot(x, 'r'); xlim([0, len]); title('Original')
subplot(2,1,2); plot(y, 'b'); xlim([0, len]); title('Stretched')

%% Change Pitch
speed = pvoc(x, 0.8); % 4/5 speed
pitch = resample(speed, 4, 5); % resample(input, numerator, denominator), change pitch
len = max(length(speed), length(pitch)); % longer signal

% Play
% pause() % press any key
% soundsc(x(1:length(pitch)) + pitch, sr) % overlap original with pitch shifted

% Plot
figure
subplot(3,1,1); plot(x, 'm'); xlim([0, len]); title('Original')
subplot(3,1,2); plot(speed, 'b'); xlim([0, len]); title('Time Shift')
subplot(3,1,3); plot(pitch, 'g'); xlim([0, len]); title('Pitch Shift (resampled)')