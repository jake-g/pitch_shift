/****************************************************************************
*

* NAME: PitchShift.h
* VERSION: 1.2
* HOME URL: http://www.dspdimension.com
*
* SYNOPSIS: Routine for doing pitch shifting while maintaining
* duration using the Short Time Fourier Transform.
*
* DESCRIPTION: The routine takes a pitchShift factor value which is between 0.5
* (one octave down) and 2. (one octave up). A value of exactly 1 does not change
* the pitch. numSamps tells the routine how many samples in indata[0...
* numSamps-1] should be pitch shifted and moved to outdata[0 ...
* numSamps-1]. The two buffers can be identical (ie. it can process the
* data in-place). fftFrameSize defines the FFT frame size used for the
* processing. Typical values are 1024, 2048 and 4096. It may be any value <=
* MAX_FRAME_LENGTH but it MUST be a power of 2. osamp is the STFT
* oversampling factor which also determines the overlap between adjacent STFT
* frames. It should at least be 4 for moderate scaling ratios. A value of 32 is
* recommended for best quality. sampleRate takes the sample rate for the signal
* in unit Hz, ie. 44100 for 44.1 kHz audio. The data passed to the routine in
* indata[] should be in the range [-1.0, 1.0), which is also the output range
* for the data, make sure you scale the data accordingly (for 16bit signed integers
* you would have to divide (and multiply) by 32768).
*
*****************************************************************************/
#ifndef PITCH_SHIFT_H
#define PITCH_SHIFT_H

#include <string.h>
#include <math.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MAX_FRAME_LENGTH 256
void fft_custom(float *fftBuffer, long fftFrameSize, long sign);
double Atan2(double x, double y);

void PitchShift(float pitchShift, long numSamps, long fftFrameSize, long osamp, float sampleRate, float *indata, float *outdata)
{
	static float inQueue[MAX_FRAME_LENGTH];
	static float outQueue[MAX_FRAME_LENGTH];
	static float windowedQueue[2*MAX_FRAME_LENGTH];
	static float lastPhase[MAX_FRAME_LENGTH/2+1];
	static float outAccum[2*MAX_FRAME_LENGTH];
	static float inFrequencies[MAX_FRAME_LENGTH];
	static float inMagnitudes[MAX_FRAME_LENGTH];
	static float outFrequencies[MAX_FRAME_LENGTH];
	static float outMagnitudes[MAX_FRAME_LENGTH];
	static long queueCounter = 0, initFlag = 0;
	double magn, phase, tmp, window, real, imag, sampsPerBin, exp;
	long i,k, phaseChange, index, inputLatency, stepSize;


	/* set up some handy variables */
	stepSize = fftFrameSize/osamp;
	sampsPerBin = sampleRate/(double)fftFrameSize;
	exp = 2.*M_PI*(double)stepSize/(double)fftFrameSize;
	inputLatency = fftFrameSize-stepSize;
	if (queueCounter == 0) queueCounter = inputLatency;

	/* initialize our static arrays */
	if (initFlag == 0) {
		memset(inQueue, 0, MAX_FRAME_LENGTH*sizeof(float));
		memset(outQueue, 0, MAX_FRAME_LENGTH*sizeof(float));
		memset(windowedQueue, 0, 2*MAX_FRAME_LENGTH*sizeof(float));
		memset(lastPhase, 0, (MAX_FRAME_LENGTH/2+1)*sizeof(float));
		memset(outAccum, 0, 2*MAX_FRAME_LENGTH*sizeof(float));
		memset(inFrequencies, 0, MAX_FRAME_LENGTH*sizeof(float));
		memset(inMagnitudes, 0, MAX_FRAME_LENGTH*sizeof(float));
		initFlag = 1;
	}

	/* main processing loop */
	for (i = 0; i < numSamps; i++){

		/* As long as we have not yet collected enough data just read in */
		inQueue[queueCounter] = indata[i];
		outdata[i] = outQueue[queueCounter-inputLatency];
		queueCounter++;

		/* now we have enough data for processing */
		if (queueCounter >= fftFrameSize) {
			queueCounter = inputLatency;

			/* do windowing and re,im interleave */
			for (k = 0; k < fftFrameSize;k++) {
				window = -.5*cos(2.*M_PI*(double)k/(double)fftFrameSize)+.5;
				windowedQueue[2*k] = inQueue[k] * window;
				windowedQueue[2*k+1] = 0.;
			}


			/* ***************** ANALYSIS ******************* */
			/* do transform */
			fft_custom(windowedQueue, fftFrameSize, -1);

			/* this is the analysis step */
			for (k = 0; k <= fftFrameSize/2; k++) {

				/* de-interlace FFT buffer */
				real = windowedQueue[2*k];
				imag = windowedQueue[2*k+1];

				/* compute magnitude and phase */
				magn = 2.*sqrt(real*real + imag*imag);
				phase = Atan2(imag,real);

				/* compute phase difference */
				tmp = phase - lastPhase[k];
				lastPhase[k] = phase;

				/* subtract expected phase difference */
				tmp -= (double)k*exp;

				/* map delta phase into +/- Pi interval */
				phaseChange = tmp/M_PI;
				if (phaseChange >= 0) phaseChange += phaseChange&1;
				else phaseChange -= phaseChange&1;
				tmp -= M_PI*(double)phaseChange;

				/* get deviation from bin frequency from the +/- Pi interval */
				tmp = osamp*tmp/(2.*M_PI);

				/* compute the k-th partials' true frequency */
				tmp = (double)k*sampsPerBin + tmp*sampsPerBin;

				/* store magnitude and true frequency in analysis arrays */
				inMagnitudes[k] = magn;
				inFrequencies[k] = tmp;

			}

			/* ***************** PROCESSING ******************* */
			/* this does the actual pitch shifting */
			memset(outMagnitudes, 0, fftFrameSize*sizeof(float));
			memset(outFrequencies, 0, fftFrameSize*sizeof(float));
			for (k = 0; k <= fftFrameSize/2; k++) {
				index = k*pitchShift;
				if (index <= fftFrameSize/2) {
					outMagnitudes[index] += inMagnitudes[k];
					outFrequencies[index] = inFrequencies[k] * pitchShift;
				}
			}

			/* ***************** SYNTHESIS ******************* */
			/* this is the synthesis step */
            phase = 0;
			for (k = 0; k <= fftFrameSize/2; k++) {

				/* get magnitude and true frequency from synthesis arrays */
				magn = outMagnitudes[k];
				tmp = outFrequencies[k];

				/* subtract bin mid frequency */
				tmp -= (double)k*sampsPerBin;

				/* get bin deviation from freq deviation */
				tmp /= sampsPerBin;

				/* take osamp into account */
				tmp = 2.*M_PI*tmp/osamp;

				/* add the overlap phase advance back in */
				tmp += (double)k*exp;

				/* accumulate delta phase to get bin phase */
				phase += tmp;

				/* get real and imag part and re-interleave */
				windowedQueue[2*k] = magn*cos(phase);
				windowedQueue[2*k+1] = magn*sin(phase);
			}

			/* zero negative inFrequencies */
			for (k = fftFrameSize+2; k < 2*fftFrameSize; k++) windowedQueue[k] = 0.;

			/* do inverse transform */
			fft_custom(windowedQueue, fftFrameSize, 1);

			/* do windowing and add to output accumulator */
			for(k=0; k < fftFrameSize; k++) {
				window = -.5*cos(2.*M_PI*(double)k/(double)fftFrameSize)+.5;
				outAccum[k] += 2.*window*windowedQueue[2*k]/(fftFrameSize/2*osamp);
			}
			for (k = 0; k < stepSize; k++) outQueue[k] = outAccum[k];

			/* shift accumulator */
			memmove(outAccum, outAccum+stepSize, fftFrameSize*sizeof(float));

			/* shift input queue*/
			for (k = 0; k < inputLatency; k++) inQueue[k] = inQueue[k+stepSize];
		}
	}
}


void fft_custom(float *fftBuffer, long fftFrameSize, long sign)
/*
	FFT routine: Sign = -1 is FFT, 1 is iFFT (inverse)
	Fills fftBuffer[0...2*fftFrameSize-1] with the Fourier transform of the
	time domain data in fftBuffer[0...2*fftFrameSize-1]. The FFT array takes
	and returns the cosine and sine parts in an interleaved manner, ie.
	fftBuffer[0] = cosPart[0], fftBuffer[1] = sinPart[0], asf. fftFrameSize
	must be a power of 2. It expects a complex input signal (see footnote 2),
	ie. when working with 'common' audio signals our input signal has to be
	passed as {in[0],0.,in[1],0.,in[2],0.,...} asf. In that case, the transform
	of the inFrequencies of interest is in fftBuffer[0...fftFrameSize].
*/
{
	float wr, wi, arg, *p1, *p2, temp;
	float tr, ti, ur, ui, *p1r, *p1i, *p2r, *p2i;
	long i, bitm, j, le, le2, k;

	for (i = 2; i < 2*fftFrameSize-2; i += 2) {
		for (bitm = 2, j = 0; bitm < 2*fftFrameSize; bitm <<= 1) {
			if (i & bitm) j++;
			j <<= 1;
		}
		if (i < j) {
			p1 = fftBuffer+i; p2 = fftBuffer+j;
			temp = *p1; *(p1++) = *p2;
			*(p2++) = temp; temp = *p1;
			*p1 = *p2; *p2 = temp;
		}
	}
	for (k = 0, le = 2; k < (long)(log(fftFrameSize)/log(2.)+.5); k++) {
		le <<= 1;
		le2 = le>>1;
		ur = 1.0;
		ui = 0.0;
		arg = M_PI / (le2>>1);
		wr = cos(arg);
		wi = sign*sin(arg);
		for (j = 0; j < le2; j += 2) {
			p1r = fftBuffer+j; p1i = p1r+1;
			p2r = p1r+le2; p2i = p2r+1;
			for (i = j; i < 2*fftFrameSize; i += le) {
				tr = *p2r * ur - *p2i * ui;
				ti = *p2r * ui + *p2i * ur;
				*p2r = *p1r - tr; *p2i = *p1i - ti;
				*p1r += tr; *p1i += ti;
				p1r += le; p1i += le;
				p2r += le; p2i += le;
			}
			tr = ur*wr - ui*wi;
			ui = ur*wi + ui*wr;
			ur = tr;
		}
	}
}

/* Atan2 function (more robust than build-in atan2) */
double Atan2(double x, double y)
{
  double signx;
  if (x > 0.) signx = 1.;
  else signx = -1.;

  if (x == 0.) return 0.;
  if (y == 0.) return signx * M_PI / 2.;

  return atan2(x, y);
}

#endif

