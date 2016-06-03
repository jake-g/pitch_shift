# EE 443 Final Project

# Pitch Shift - Delay Based Approach

## Contributers:
- Jake Garrison
- Jisoo Jung

## Hardware Interface:
- KEY0: Reset pitch factor to 1 (equivalent to shifting 0 semitone)
- KEY1: Decrease pitch factor by 0.1 if switch0 is disabled, by 1 semitone if switch0 is enabled
- KEY2: Increase pitch factor by 0.1 if switch0 is disabled, by 1 semitone if switch0 is enabled
- KEY3: UART send button. Send pitch factor and switch config.
- Switch0: Semitone toggle. High -> pitch shift by 1 semitone. Low -> pitch shift by 0.1
- Switch1: Melody toggle. Pitch shift by designated melody.
- Switch2: Delay pitch function
- Switch3: Delay echo function
- Switch4: Loop 

NOTE: Only one of switches 1 through 4 should be enabled. Others get disabled

## Software Interface: 
- GUI: either python or matlab 
