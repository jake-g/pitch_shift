input = audioread('../sound_files/Maple_short.wav');
output = audioread('../sound_files/Maple_short-out_(5).wav');

% n = 0:0.1:100;
% input = sin(n)
% output = sin(n) + 0.5*sin(5*n) + 0.3*sin(10*n)

figure; subplot(211); 
plot(abs(fftshift(fft(input))), 'g');
title('input')
subplot(212); 
plot(abs(fftshift(fft(output))), 'r');
title('output')


figure; subplot(211); 
spectrogram(input);
title('input')
subplot(212); 
spectrogram(output);
title('output')