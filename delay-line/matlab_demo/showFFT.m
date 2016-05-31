function showFFT( input, output, Fs )

figure; subplot(211); 
[y, x] = getFFT(input, Fs);
plot(x, y, 'g')
title('input')
subplot(212); 
[y, x] = getFFT(output, Fs);
plot(x, y, 'r')
title('output')


figure; subplot(211); 
spectrogram(input);
title('input')
subplot(212); 
spectrogram(output);
title('output')
end