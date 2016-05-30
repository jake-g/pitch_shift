/////////////////////////////////////////////////////////////////////
//
// Simple SOLA algorithm example. The example reads a .wav sound 
// file with mono-16bit-44100Hz sample format, process it with SOLA 
// and writes output into another .wav file.
// 
// Copyright (c) Olli Parviainen 2006 <oparviai@iki.fi>
//
/////////////////////////////////////////////////////////////////////

#include <stdexcept>
#include "wavfile.h"

using namespace std;

// Time scaling factor, values > 1.0 increase, values < 1.0 decrease tempo
#define TIME_SCALE      0.85   // 15% slower tempo
// Processing sequence size (100 msec with 44100Hz samplerate)
#define SEQUENCE        4410
// Overlapping size (20 msec)
#define OVERLAP         882
// Best overlap offset seeking window (15 msec)
#define SEEK_WINDOW     662
// Processing sequence flat mid-section duration
#define FLAT_DURATION   (SEQUENCE - 2 * (OVERLAP))
// Theoretical interval between the processing seqeuences
#define SEQUENCE_SKIP   ((int)((SEQUENCE - OVERLAP) * (TIME_SCALE)))

typedef short SAMPLE;   // sample type, 16bit signed integer

// Use cross-correlation function to find best overlapping offset 
// where input_prev and input_new match best with each other
int seek_best_overlap(const SAMPLE *input_prev, const SAMPLE *input_new)
{
   int i;
   int bestoffset = 0;
   float bestcorr = -1e30f;
   float temp[OVERLAP];

   // Precalculate overlapping slopes with input_prev
   for (i = 0; i < OVERLAP; i ++)
   {
      temp[i] = (float)(input_prev[i] * i * (OVERLAP - i));
   }

   // Find best overlap offset within [0..SEEK_WINDOW]
   for (i = 0; i < SEEK_WINDOW; i ++)
   {
      int j;
      float crosscorr = 0;

      for (j = 0; j < OVERLAP; j ++)
      {
         crosscorr += (float)input_new[i + j] * temp[j];
      }
      if (crosscorr > bestcorr)
      {
         // found new best offset candidate
         bestcorr = crosscorr;
         bestoffset = i;
      }
   }
   return bestoffset;
}


// Overlap 'input_prev' with 'input_new' by sliding the amplitudes during 
// OVERLAP samples. Store result to 'output'.
void overlap(SAMPLE *output, const SAMPLE *input_prev, const SAMPLE *input_new)
{
   int i;

   for (i = 0; i < OVERLAP; i ++)
   {
      output[i] = (input_prev[i] * (OVERLAP - i) + input_new[i] * i) / OVERLAP;
   }
}


// SOLA algorithm. Performs time scaling for sample data given in 'input', 
// write result to 'output'. Return number of output samples.
int sola(SAMPLE *output, const SAMPLE *input, int num_in_samples)
{
   int num_out_samples = 0;
   const SAMPLE *seq_offset = input;
   const SAMPLE *prev_offset;

   while (num_in_samples > SEQUENCE_SKIP + SEEK_WINDOW)
   {
      // copy flat mid-sequence from current processing sequence to output
      memcpy(output, seq_offset, FLAT_DURATION * sizeof(SAMPLE));
      // calculate a pointer to overlap at end of the processing sequence
      prev_offset = seq_offset + FLAT_DURATION;

      // update input pointer to theoretical next processing sequence begin
      input += SEQUENCE_SKIP - OVERLAP;
      // seek actual best matching offset using cross-correlation
      seq_offset = input + seek_best_overlap(prev_offset, input);

      // do overlapping between previous & new sequence, copy result to output
      overlap(output + FLAT_DURATION, prev_offset, seq_offset);

      // Update input & sequence pointers by overlapping amount
      seq_offset += OVERLAP;
      input  += OVERLAP;

      // Update output pointer & sample counters
      output += SEQUENCE - OVERLAP;
      num_out_samples += SEQUENCE - OVERLAP;
      num_in_samples -= SEQUENCE_SKIP;
   }

   return num_out_samples;
}



// Buffers for input/output sample data. For sake of simplicity, these are 
// just made 'big enough' for the example purpose.
SAMPLE inbuffer[10240000];
SAMPLE outbuffer[20240000];

int main(int numstr, char **pstr)
{

   if (numstr < 3)
   {
      printf("usage: solatest input.wav output.wav\n");
      return -1;
   }

   try
   {
      int insamples, outsamples;

      // Open input file
      WavInFile infile(pstr[1]);

      if ((infile.getSampleRate() != 44100) || (infile.getNumChannels() != 1))
      {
         printf("Sorry, this example processes mono audio sampled at 44100Hz.\n");
         return -1;
      }

      // Read data from input file
      insamples = infile.read(inbuffer, 10240000);

      // Process
      outsamples = sola(outbuffer, inbuffer, insamples);

      // Write result to output file
      WavOutFile outfile(pstr[2], infile.getSampleRate(), infile.getNumBits(), infile.getNumChannels());
      outfile.write(outbuffer, outsamples);
   } 
   catch (exception &e)
   {
      printf("Error: %s\n", e.what());
   }

   return 0;
}
