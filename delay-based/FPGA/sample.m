% HW1 Question 5
% Jake and Jisoo

% close all; clc; clear all;

%% Read UART
delete(instrfindall);
s = serial('COM6', 'BaudRate',115200); % Open the serial port to receive the data
set(s,'InputBufferSize',20000); % set the size of input buffer
fopen(s); % get ready to receive the data
buffersize = 2; % set the size of instant read of buffer
delay = 0.25

while(1)
    buffer = fread(s,buffersize, 'int16'); % read the buffer when data arrive
    pInt = buffer(1);  
    sw = buffer(2)
    pitch = pInt/10000;
    semitone = log2(pitch)*12
    pause(delay)
end