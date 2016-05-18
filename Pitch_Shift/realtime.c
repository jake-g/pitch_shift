
/*
 * Reads a buffer from the OS input device (microphone)
 * Applies processing via fxCallback
 * Outputs audio to OS output (speakers/headphones)
 *
 *
 * This program uses the PortAudio Portable Audio Library.
 * For more information see: http://www.portaudio.com
 * Copyright (c) 1999-2000 Ross Bencina and Phil Burk
*/

#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#include "PitchShift.h"


#define SAMPLE_RATE         (44100)
#define NUM_CHANNELS        (1)
#define PA_SAMPLE_TYPE      paFloat32
#define FRAMES_PER_BUFFER   (64)
typedef float SAMPLE;


float CubicAmplifier( float input );
void printBuffer(const float * arr);
int main(void);
static int fxCallback( const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData );

long semitones = 3;

int count = 0;
// Calls the effect on input buffer and returns output buffer when complete
static int fxCallback( const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData )
{
    // Port audio variables
    SAMPLE *out = (SAMPLE*)outputBuffer;
    SAMPLE *tmp_buff = (SAMPLE*)outputBuffer;
    const SAMPLE *in = (const SAMPLE*)inputBuffer;
    unsigned int i;
    (void) timeInfo; /* Prevent unused variable warnings. */
    (void) statusFlags;
    (void) userData;

    // Pitch Shift variables
    long buffer_size = FRAMES_PER_BUFFER;
    float sr = SAMPLE_RATE;
    long fftSize = 2048;
    long osamp = 32;
    float pitchShift = pow(2., semitones/12.);
    int measure = 0;

    // Fill input buffer
    if( inputBuffer == NULL )
    {
        for( i=0; i<framesPerBuffer; i++ )
        {
            *out++ = 0;  /* left - silent */
        }
    }
    else    // playback
    {
        // Buffer based effect
        PitchShift(pitchShift, buffer_size , fftSize, osamp, sr, (float *)in, out);

         // Melody
         measure = 10000;
         if (count == measure) {
           semitones = 0;
           printf("Setting semitone to %ld...\n", semitones);
         } else if (count == measure * 1/8) {
           semitones += 4;
           printf("Setting semitone to %ld...\n", semitones);
         } else if (count == measure * 2/4) {
           semitones += 3;
           printf("Setting semitone to %ld...\n", semitones);
         } else if (count == measure * 3/4) {
           semitones += 4;
           printf("Setting semitone to %ld...\n", semitones);
         }

    }
    // printf("%d\n", count );
count = (count + 1) % measure;

    return paContinue;
}

/*******************************************************************/
int main()
{
    PaStreamParameters inputParameters, outputParameters;
    PaStream *stream;
    PaError err;

    err = Pa_Initialize();
    if( err != paNoError ) goto error;

    inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    if (inputParameters.device == paNoDevice) {
      fprintf(stderr,"Error: No default input device.\n");
      goto error;
    }
    inputParameters.channelCount = NUM_CHANNELS;       /* stereo input */
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if (outputParameters.device == paNoDevice) {
      fprintf(stderr,"Error: No default output device.\n");
      goto error;
    }
    outputParameters.channelCount = NUM_CHANNELS;       /* stereo output */
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(
              &stream,
              &inputParameters,
              &outputParameters,
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              0, /* paClipOff, */  /* we won't output out of range samples so don't bother clipping them */
              fxCallback,
              NULL );
    if( err != paNoError ) goto error;

    err = Pa_StartStream( stream );
    if( err != paNoError ) goto error;

    printf("Hit ENTER to stop program.\n");
    getchar();
    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto error;

    printf("Finished...\n");
    Pa_Terminate();
    return 0;

error:
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return -1;
}

/* Print contents of data (float array 1 row per channel) */
void printBuffer(const float *arr)
{
    printf("fresh buffer...\n");
    for (int i = 0; i < FRAMES_PER_BUFFER; i++) {
        printf("%f ", arr[i]);
    }
    printf("\n");

}
