function [ xfft, freqs ] = getFFT( x, Fs )
t = 0:1/Fs:1-1/Fs;
xfft = fft(x);
xfft = abs(xfft(1:length(x)/2+1));
DF = Fs/length(x); % frequency increment
freqs = 0:DF:Fs/2;

end

