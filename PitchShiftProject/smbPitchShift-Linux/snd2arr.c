#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>

/*
Requres:
sudo apt-get install libsndfile1
sudo apt-get install libsndfile1-dev

Build:
gcc snd2arr.c -o snd2arr -lsndfile

*/

int main()
    {
    SNDFILE *sf;
    SF_INFO info;
    int num_channels;
    int num, num_items;
    int *buf;
    int f,sr,c;
    int i,j;
    FILE *out;

    /* Open the WAV file. */
    info.format = 0;
    sf = sf_open("file.wav",SFM_READ,&info);
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
    num_items = f*c;
    printf("num_items=%d\n",num_items);
    /* Allocate space for the data to be read, then read it. */
    buf = (int *) malloc(num_items*sizeof(int));
    num = sf_read_int(sf,buf,num_items);
    sf_close(sf);
    printf("Read %d items\n",num);
    /* Write the data to filedata.out. */
    out = fopen("filedata.out","w");
    for (i = 0; i < num; i += c)
        {
        for (j = 0; j < c; ++j)
            fprintf(out,"%d ",buf[i+j]);
        fprintf(out,"\n");
        }
    fclose(out);
    return 0;
    }
