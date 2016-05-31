%delay-based pitch shifting

%output = pitch_shift(file,sign,semitones)
%file- wavefile with input
%sign 0-shifts pitch down 
%               1-shifts pitch up
%semitones - number of semitones to shift pitch,
%                deafualt values is 6, half an octave
%                past one octave of shift artifacts become more noticable
%output - pitch shifted input

function output = pitch_shift(file, semitones)


[input,fs] = audioread(file);
inputLength = size(input,1);
output = zeros(inputLength,1);

% Set sign flag
if (semitones < 0)
    sign = 0
else 
    sign = 1
end

% Constants in ms
buffer_depth_ms = 40e-3;
delay_depth_up_ms = 12e-3;
initial_delay_up_ms = 30e-3; % delay for up shift
initial_delay_down_ms = 0.1e-3; % delay for down shift
delay_depth_down_ms=initial_delay_up_ms-delay_depth_up_ms+initial_delay_down_ms;

%convert constants to samples
pitch_shift_rate = abs(2^(semitones/12)-1);
buffer_depth = round(buffer_depth_ms * (fs));
delay_depth_up = round(delay_depth_up_ms * (fs));
delay_depth_down = round(delay_depth_down_ms * (fs));
initial_delay_up = round(initial_delay_up_ms * fs);
initial_delay_down = round(initial_delay_down_ms * fs);
crossfading = 0;
crossfadingAB = 0; % 0 B to A; 1 A to B
q = 1;

% Create Channel A & B buffers
bufferA = zeros(buffer_depth,1);
bufferB = zeros(buffer_depth,1);
% pointers to buffers, used for variable delay
% These pointers determine the end of the buffer, where we read our output from.
bufferAptr = 1;
bufferBptr = 1;

% Initialize Channel Gains, Outputs
Ga = 1;
Gb = 0;
Aout = 0;
Bout = 0;

% Initialize Delay according to type of pitch shift.
if (sign)
    bufferAptr = initial_delay_up; % Initialize delay ~25ms
    bufferBptr = initial_delay_up;
else
    bufferAptr = initial_delay_down;   % Initialize delay ~0ms
    bufferBptr = initial_delay_down;
end

disp('Shifting...');
for n = 1:inputLength
    % Shift the input into the buffer
    bufferA(2:buffer_depth) = bufferA(1:buffer_depth-1); % like a stack
    bufferB(2:buffer_depth) = bufferB(1:buffer_depth-1);
    bufferA(1) = input(n);
    bufferB(1) = input(n);
    
    % Compute Channel Outputs & system output
    Aout = Ga * bufferA(round(bufferAptr));  % Simplified model for now.
    Bout = Gb * bufferB(round(bufferBptr));
    output(n) = Aout + Bout;
    
    % Check if we need to crossfade to other channel (FALSE if any are 0)
    if ((sign == 1)&((round(bufferAptr) <= delay_depth_up)+(round(bufferBptr) <= delay_depth_up))&(crossfading ~= 1))
        crossfading = 1;
        crossfadingAB = not(crossfadingAB);
        q = 1;  % This variable controls the fade between channels.
    elseif ((sign == 0)&((round(bufferAptr) >= delay_depth_down)+(round(bufferBptr) >= delay_depth_down))&(crossfading ~= 1))
        crossfading = 1;
        crossfadingAB = not(crossfadingAB);
        q = 1;
    end
    
    % Are we crossfading? If so, adjust Channel Gains accordingly
    % Crossfades alternate between AB and BA
    if (crossfading)
        % check which direction we are crossfading
        if (crossfadingAB) % crossfade A to B
            Ga=cos((pitch_shift_rate/delay_depth_up)*(pi/2)*q);%smooth sinusoidal crossfade
            Gb = 1-Ga;  % eventually Gb > Ga (slowly), then the crossfade ends
            q = q + 1;
        else % crossfade B to A
            Gb = cos((pitch_shift_rate/delay_depth_up)*(pi/2)*q);
            Ga = 1-Gb;
            q = q + 1;
        end
        % Are we done crossfading?  If so, stop, reset appropriate delay
        if ((q==round(delay_depth_up/pitch_shift_rate))&(Ga<Gb)) % A to B crossfade done
            Ga = 0;
            Gb = 1;
            q = 1;
            crossfading = 0;
            if (sign)
                bufferAptr = initial_delay_up;
            else
                bufferAptr = initial_delay_down;
            end
        end
        if ((q==round(delay_depth_up/pitch_shift_rate))&(Ga>Gb)) % B to A crossfade done
            Gb = 0;
            Ga = 1;
            q = 1;
            crossfading = 0;
            if (sign)
                bufferBptr = initial_delay_up;
            else
                bufferBptr = initial_delay_down;
            end
        end
    end
    % Change channel delays according to desired pitch shift
    if (sign)
        if (Ga ~= 0)  % only change pointers for active channels
            bufferAptr = bufferAptr - pitch_shift_rate;
        end
        if (Gb ~= 0)
            bufferBptr = bufferBptr - pitch_shift_rate;
        end
    else
        if (Ga ~= 0)  % only change pointers for active channels
            bufferAptr = bufferAptr + pitch_shift_rate;
        end
        if (Gb ~= 0)
            bufferBptr = bufferBptr + pitch_shift_rate;
        end
    end
end      
disp('done');


