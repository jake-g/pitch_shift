/* *********************************************************************************

	EXAMPLE main() for smbPitchShift using MiniAiff to handle sound file i/o
	(c) 2003-2011 S. M. Bernsee, http://www.dspdimension.com
	
	IMPORTANT: requires miniAIFF to be included in the project

********************************************************************************* */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "MiniAiff.h"



void smbPitchShift(float pitchShift, long numSampsToProcess, long fftFrameSize, long osamp, float sampleRate, float *indata, float *outdata);


int main(void)
{
	
	long numChannels = 1;
	long bufferLengthFrames = 8192;
	int ret = 0;
	
	float **data = mAiffAllocateAudioBuffer(numChannels, bufferLengthFrames);
	
	long semitones = 3;							// shift up by 3 semitones
	float pitchShift = pow(2., semitones/12.);	// convert semitones to factor
	
	char inFileName[] = "voice.aif";
	char outFileName[] = "out.aif";

	printf("Running MiniAiff version %s\n(C) S.M.Bernsee - http://www.dspdimension.com\n\n", mAiffGetVersion());


	// Create output file
	printf("Creating output\n");
	ret = mAiffInitFile(outFileName, 44100. /* sample rate */, 16 /* bits */, numChannels /* mono */);
	if (ret < 0) {
		printf("! Error creating output file %s - error #%d\n! File may be busy\n", outFileName, ret);
		exit(-1);
	}

	unsigned long inPosition=0;
	for(;;)
	{
	
		// Read from input file
		printf("Reading from input: %s\n", inFileName);
		ret = mAiffReadData(inFileName, data, inPosition, bufferLengthFrames, numChannels);
		if (ret < 0) {
			printf("! Error reading input file %s - error #%d\n! Wrong format or filename\n", inFileName, ret);
			exit(-1);
		} else if (ret == 0) break;		// end of file reached
		
		// Increment the read position
		inPosition += bufferLengthFrames;

		// --------------------------------- call smbPitchShift() ---------------------------------
		smbPitchShift(pitchShift, bufferLengthFrames, 2048, 4, mAiffGetSampleRate(inFileName), data[0], data[0]);
		// ----------------------------------------------------------------------------------------

		// write processed data to output file
		printf("Writing to output: %s\n", outFileName);
		ret = mAiffWriteData(outFileName, data, bufferLengthFrames, numChannels);
		if (ret < 0) {
			printf("! Error writing output file %s - error #%d\n! Not enough space?\n", outFileName, ret);
			exit(-1);
		}
	}

	mAiffDeallocateAudioBuffer(data, numChannels);
	
	// That's all there is to it.
	printf("You're a cool dude now!\n");
	
	system("open out.aif");

	
	return 0;
}

