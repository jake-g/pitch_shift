close all; clc; clear all;
% delete(instrfindall);
s = serial('COM4', 'BaudRate',115200); % Open the serial port to receive the data
set(s,'InputBufferSize',20000); % set the size of input buffer
fopen(s); % get ready to receive the data
buffersize = 8002; % set the size of instant read of buffer

buffer = fread(s,buffersize, 'int16'); % read the buffer when data arrive

pInt = buffer(1);  
sw = buffer(2)
pitch = pInt/10000;
semitone = round(log2(pitch)*12)

x = buffer(3:end);
fclose(s);
delete(s);

%%
% input = x(1:256);
% output = x(257:end);
output = x;
N = 8000;
%----- Output ---------
plot(output);
figure;
b = (abs(fft(output, N))/N);
range = linspace(-N/2, N/2, N);
plot(range, fftshift(b));

xlim([-5000 5000])
xlabel('Frequency (KHz)')	 
title('FFT');	 	 
xlabel('Frequency (Hz)')	 	 
ylabel('Magnitude');
