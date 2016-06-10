# EE 443 Final Project

# Pitch Shift - Delay Based Approach

## Contributers:
- Jake Garrison
- Jisoo Jung

<!--## Hardware Interface:-->
<!--- KEY0: Reset pitch factor to 1 (equivalent to shifting 0 semitone)-->
<!--- KEY1: Decrease pitch factor by 0.1 if switch0 is disabled, by 1 semitone if switch0 is enabled-->
<!--- KEY2: Increase pitch factor by 0.1 if switch0 is disabled, by 1 semitone if switch0 is enabled-->
<!--- KEY3: UART send button. Send pitch factor and switch config.-->
<!--- Switch0: Semitone toggle. High -> pitch shift by 1 semitone. Low -> pitch shift by 0.1-->
<!--- Switch1: Melody toggle. Pitch shift by designated melody.-->
<!--- Switch2: Delay pitch function-->
<!--- Switch3: Delay echo function-->
<!--- Switch4: Loop -->
<!--- Switch5: Disable right channel dry sound-->
<!--- Switch7: Toggle input-->

<!--NOTE: Only one of switches 1 through 4 should be enabled. Others get disabled-->

## Hardware Control
### Pitch Shift Control

- **KEY0:** Pitch reset (p = 1, semitone = 0, no shift)
- **SW0 OFF:** pitch factor (inharmonic multiplier)
  - KEY1: Pitch shift decrement 0.1
  - KEY2: Pitch shift increment 0.1
- **SW0 ON:** semitone mode (half step mapping)
  - KEY1: Semitone decrement 1
  - KEY2: Semitone increment 1

### I/O Control

- **SW5 ON:** Enable dry input routing to right output channel
- **SW5 OFF:** Disable right output channel (Wet only)
- **SW7 ON:** Microphone input
- **SW7 OFF:** Line Input

### UART Control

_Sends data to our GUI_

- **KEY3:** Send UART buffer (current pitch factor, switch config and audio buffer)

### Effects Control

_SW1 – SW4: ON is enabled, OFF is disabled, only one can be enabled_

- **SW1 Melody Auto tune:** pitch changes according to user's melody and tempo
  - Keys control pitch shift
- **SW2 Pitch Decay:** Input is echoed, pitch shifting each echo by a given rate
  - KEY0: decrease pitch change rate
  - KEY1: increase pitch change rate
- **SW3 Echo:** Input is echoed with constant pitch and variable decay
  - KEY0: decrease decay rate
  - KEY1: increase decay rate
- **SW4 Loop:** Loop input indefinitely
  - Keys control pitch shift

## Software Interface:
GUI: Matlab
![alt tag](https://github.com/jake-g/audio_fx/blob/master/delay-based/GUI/matlab_GUI.PNG)
1. **Global settings:** Displays current note as well as global tempo and key
  * User an input tempo and key note frequency

1. **Waveform select:** Choose waveform to map to piano keys

2. **Sample Mode:** Maps sample (from UART) to piano keys using pitch shift algorithm
  * User can input sample start and end points

3. **Melody Controls:** Play, record or reset a melody

4. **Note length:** Sets duration of next note played

5. **UART Control:** Initialize and receive UART
  * User can set COM port
  * Receive, then press KEY3 on hardware to send switch config, current note and an audio sample
  * Audio sample is used when in Sample Mode

6. **Hardware Status:** Indicates current pitch setting and Effects switch configuration
