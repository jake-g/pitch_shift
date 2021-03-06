#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>

/*
This code will read a wav and print the raw values -- Jake
---------------------------------------------------------

Requres:
sudo apt-get install libsndfile1
sudo apt-get install libsndfile1-dev

Build:
gcc snd2arr.c -o snd2arr -lsndfile

Notes: libsndfile
http://www.mega-nerd.com/libsndfile/api.html
http://www.labbookpages.co.uk/audio/wavFiles.html
*/


int main(void)
    {
    SNDFILE *sf;
    SF_INFO info;
    int num_channels;
    int num, buffer_size;
    float *buf;
    int f, sr, c;

  	char inFileName[] = "voice.aif";

    /* Open the WAV file. */
    info.format = 0;
    sf = sf_open(inFileName, SFM_READ,&info);
    if (sf == NULL)
        {
        printf("Failed to open the file.\n");
        exit(-1);
        }

    /* Print some of the info, and figure out how much data to read. */
    f = info.frames;
    sr = info.samplerate;
    c = info.channels;
    printf("frames=%d\n",f);
    printf("samplerate=%d\n",sr);
    printf("channels=%d\n",c);
    buffer_size = f*c;
    printf("buffer_size=%d\n",buffer_size);

    /* Allocate space for the data to be read, then read it. */
    buf = (float *) malloc(buffer_size*sizeof(float));
    num = sf_readf_float(sf, buf, buffer_size);
    sf_close(sf);
    printf("Read %d intput items\n", num);

    /* Print contents of data (float array 1 row per channel)*/
      int i;
  		for (i = 0; i < buffer_size; i++) {
  			printf("%f\t", buf[i]);
  		}


    return 0;
    }
