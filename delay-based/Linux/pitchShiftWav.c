#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include <string.h>
#include <math.h>
#include "PitchShiftFile.h"

#define NAME_SIZE 100
/*

Shifts pitch by given factor. Input and output are .wav files


On Linux:
sudo apt-get install libsndfile1
sudo apt-get install libsndfile1-dev

Build:
run makefile

Execute: ./shiftWav semitones inputFilename_with_extension

Notes: libsndfile
http://www.mega-nerd.com/libsndfile/api.html
http://www.labbookpages.co.uk/audio/wavFiles.html
*/

void printBuffer(int* arr, long size);
void PitchShiftFile(float pitch_factor, int file_size, int* inbuf, int* outbuf);
void handleInput(int argc, char** argv, int* semitones, char inFileName[], char fileExtension[]);

int main(int argc, char* argv[])
{

    /* Initialize args*/
    float sr, pitch_factor;
    int *file_buf, *buf, num, f, c, file_size, semitones;
    char inFileName[NAME_SIZE]; // file name without file extension
    char fileExtension[NAME_SIZE]; // file extension of both in/output
    char outPath[NAME_SIZE]; // format: "inputName-out.extension"
    char srcPath[NAME_SIZE]; // assuming input is in sound_files directory
    SNDFILE *input, *output;
    SF_INFO info;
    handleInput(argc, argv, &semitones, inFileName, fileExtension);
    sprintf(outPath, "sound_files/%s-out_(%d).%s", inFileName, semitones, fileExtension);
    sprintf(srcPath, "sound_files/%s.%s", inFileName, fileExtension);
    pitch_factor = pow(2., semitones / 12.);

    /* Open the WAV file. */
    info.format = 0;
    input = sf_open(srcPath, SFM_READ, &info);
    if (input == NULL) {
        printf("Failed to open the file.\n");
        exit(-1);
    }

    /* Print some of the info */
    f = info.frames;
    sr = info.samplerate;
    c = info.channels;
    file_size = f * c;
    printf("Input: samplerate: %0.0lf, channels: %d, file_size: %d\n", sr, c, file_size);
    printf("Shifted by a factor of %f\n", pitch_factor);

    /* Allocate space for the data to be read, then read it. */
    file_buf = (int*)malloc(file_size * sizeof(int));
    num = sf_readf_int(input, file_buf, file_size);
    sf_close(input);
    printf("Read %d samples from %s\n", num, srcPath);
    // printBuffer(file_buf, file_size);

    int s, bs;
    buf = (int*)malloc(file_size * sizeof(int));
    int buf_size = BUFF_SIZE;
    for (s = 0; s < file_size; s++) {
      buf[s % buf_size] = file_buf[s];

      /* Call PitchShift() */
      if (s % buf_size == 0 && s != 0) {
        // printf("s = %d\t",s - buf_size);
        PitchShiftFile(pitch_factor, buf_size, buf, buf);
        for (bs = s - buf_size; bs < s; bs++) {
            // printf("%d ", bs % buf_size);
            file_buf[bs] = buf[bs % buf_size];
        }
      }
    }


    //PitchShiftFile(pitch_factor, file_size, file_buf, file_buf);


    /*  Open sound file for writing */
    output = sf_open(outPath, SFM_WRITE, &info);
    num = sf_writef_int(output, file_buf, file_size);
    sf_close(output);
    printf("Wrote %d samples to %s\n", num, outPath);

    return 0;
}

/* Print contents of data (float array 1 row per channel) */
void printBuffer(int* arr, long size)
{
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
}

/* Parse input arguments */
void handleInput(int argc, char** argv, int* semitones, char inFileName[], char fileExtension[])
{
    if (argc != 3) {
        printf("Needs 2 arguments\n");
        exit(0);
    }

    *semitones = atoi(argv[1]);
    char del[] = ".";
    char* ptr;
    ptr = strtok(argv[2], del);
    if (ptr != NULL) {
        strcpy(inFileName, ptr);
        strcpy(fileExtension, strtok(NULL, del));
    }
}
