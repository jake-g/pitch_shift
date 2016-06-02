/*
 * Jake and Jisoo
 */

/*
 * yourISR.h
 *
 *  Created on: Mar 11, 2014
 *      Author: zhwj814
 *  Updated on : Mar 31, 2015 by jsup517
 */

#ifndef YOURISR_H_
#define YOURISR_H_

#include <string.h>
#include "PitchShift.h"
#include "system_init.h"

#define UART_BUFFER_SIZE 256
// size for ping pong buffer

void waitFor (unsigned int n) {
	printf("Pausing %d iterations...", n);
	int i;
	for (i = 0; i < n; i ++);
	printf("Finished\n");
}
// ------------------------------------------------------
int PONG[BUFF_SIZE]; // Transmit PING buffer
int PING[BUFF_SIZE]; // Transmit PONG buffer
int lastBuff[BUFF_SIZE]; // tmp buffer

int * playAndFillBuffer = NULL;
int * processBuffer = NULL;
int sampleIndex = 0;
int input_ready = 0;

short ptrStatus = 0;

float pitch_factor = 1;

int iii = 0;
// ------------------------------------------------------


//Value for interrupt ID
extern alt_u32 switch0_id;
extern alt_u32 switch1_id;
extern alt_u32 key0_id;
extern alt_u32 key1_id;
extern alt_u32 key2_id;
extern alt_u32 key3_id;
extern alt_u32 leftready_id;
extern alt_u32 rightready_id;
extern alt_u32 uart_id;

//leftChannel and rightChannel are the instant values of the value reading from ADC
extern int leftChannel;
extern int rightChannel;
extern int convResultBufferSize;
extern int convIndex;

//Default ADC Sampling frequency = 8k
extern int sampleFrequency;

//Buffer that store values from different channels
extern alt_16 leftChannelData[BUFFERSIZE];
extern alt_16 rightChannelData[BUFFERSIZE];
extern int convResultBuffer[CONVBUFFSIZE];
extern alt_16 datatest[UART_BUFFER_SIZE];

/*uart-global
 * RxHead: integer indicator tells you the index of where the
 * newest char data you received from host computer
 *
 * rx_buffer-> A ring buffer to collect uart data sent by host computer
 * */
extern unsigned short RxHead;
extern unsigned char rx_buffer[RX_BUFFER_SIZE];

/*Channel indicators: indicate the index of the most recent collected data*/
extern int leftCount;
extern int lefttestCount;
extern int rightCount;
extern int calBuffersize;
extern int leftBufferFull;

/*temporary use*/
extern int tempCount;

/*Send flag: 1->ok, now send data to the host
 *           0->ok, will not send any data to the host
 *Recv flag: 1->ok, now check and store data from the host
 *           0->ok, will not receive any data from the host
 */
extern int uartStartSendFlag;
extern int uartStartRecvFlag;
/*for uart receive purpose*/
extern int sr;

/*0->do not update sampling frequency*/
/*1->ok, update sampling frequency to AIC23*/
extern int setFreqFlag;

/*uart object*/
extern int uart;

static void handle_switch0_interrupt(void* context, alt_u32 id) {
	 volatile int* switch0ptr = (volatile int *)context;
	 *switch0ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH0_BASE);

	 /* Write to the edge capture register to reset it. */
	 IOWR_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH0_BASE, 0);

	 /*Perform Jobs*/
	 alt_irq_enable(leftready_id);
}

static void handle_switch1_interrupt(void* context, alt_u32 id) {
	 volatile int* switch1ptr = (volatile int *)context;
	 *switch1ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH1_BASE);

	 /* Write to the edge capture register to reset it. */
	 IOWR_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH1_BASE, 0);

	 /*Perform Jobs*/
}

/* Enable the flag to send recent
 * channel buffer to host computer.
 */
static void handle_key0_interrupt(void* context, alt_u32 id) {
	 volatile int* key0ptr = (volatile int *)context;
	 *key0ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(KEY0_BASE);

	 /* Write to the edge capture register to reset it. */
	 IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY0_BASE, 0);

	 uartStartSendFlag = 1;
	 pitch_factor = 1;
	 printf("pitch_factor set to 0\n");
}

/* Enable the flag to update the
 * ADC sampling frequency on AIC23.
 */
static void handle_key1_interrupt(void* context, alt_u32 id) {
	 volatile int* key1ptr = (volatile int *)context;
	 *key1ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(KEY1_BASE);

	 /* Write to the edge capture register to reset it. */
	 IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY1_BASE, 0);

	 //IOWR_ALTERA_AVALON_PIO_IRQ_MASK(SWITCH1_BASE, 0x01);
	 setFreqFlag = 1;
	 if (pitch_factor <= 0) {
		 printf("pitch_factor cannot be lower than 0\n");
	 } else {
		 pitch_factor = pitch_factor - 0.1;
		 printf("pitch_factor decreased to %f\n", pitch_factor);
	 }
}

static void handle_key2_interrupt(void* context, alt_u32 id) {
	 volatile int* key2ptr = (volatile int *)context;
	 *key2ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(KEY2_BASE);

	 /* Write to the edge capture register to reset it. */
	 IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY2_BASE, 0);
	 pitch_factor = pitch_factor + 0.1;
	 printf("pitch_factor increased to %f\n", pitch_factor);
}

static void handle_key3_interrupt(void* context, alt_u32 id) {
	 volatile int* key3ptr = (volatile int *)context;
	 *key3ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(KEY3_BASE);

	 /* Write to the edge capture register to reset it. */
	 IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY3_BASE, 0);

	 IOWR_ALTERA_AVALON_PIO_DATA(LED_BASE, 0x10);
}

/*  Detect left channel ready interrupt and do:
 *  Collect data,
 *  store in buffer,
 *  instantly play back.
 *
 */

int unsigned2signed(alt_16 unsign){
	int result;
	if(unsign>32767)
		result = unsign - 65536;
	else
		result = unsign;
	return result;
}

alt_16 signed2unsigned(int sign){
	alt_16 result;
	if(sign<0)
		result = sign + 65536;
	else
		result = sign;
	return result;
}

int last_i = 0;
static void handle_leftready_interrupt_test(void* context, alt_u32 id) {
	volatile int* leftreadyptr = (volatile int *)context;
	*leftreadyptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(LEFTREADY_BASE);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(LEFTREADY_BASE, 0);
	 /*******Read, playback, store data*******/
	if (input_ready == 0) {
		last_i = BUFF_SIZE;
		leftChannel = IORD_ALTERA_AVALON_PIO_DATA(LEFTDATA_BASE);

		// Initialize ping pong pointers
		if (playAndFillBuffer == NULL || processBuffer == NULL) {
			playAndFillBuffer = PING;
			processBuffer = PONG;
		}

		// play and fill operation
			IOWR_ALTERA_AVALON_PIO_DATA(LEFTSENDDATA_BASE, playAndFillBuffer[sampleIndex]);
			lastBuff[sampleIndex] = playAndFillBuffer[sampleIndex];
			playAndFillBuffer[sampleIndex] = leftChannel;


		// index checking for input buffer ready
		sampleIndex++;
		if (sampleIndex == BUFF_SIZE) {
			if (ptrStatus == 0) {
				playAndFillBuffer = PONG;
				processBuffer = PING;
				ptrStatus = 1;
			} else {
				playAndFillBuffer = PING;
				processBuffer = PONG;
				ptrStatus = 0;
			}

			sampleIndex = 0;
			input_ready = 1;
		}
	} else {
		IOWR_ALTERA_AVALON_PIO_DATA(LEFTSENDDATA_BASE, 	lastBuff[last_i]);
		last_i = (last_i + 1) % BUFF_SIZE;
 ;
//		sampleIndex++;
//		IOWR_ALTERA_AVALON_PIO_DATA(LEFTSENDDATA_BASE, playAndFillBuffer[iii]);
//		IOWR_ALTERA_AVALON_PIO_DATA(LEFTSENDDATA_BASE, 0);
//		iii = (iii + 1) % 1000;
//		if (iii == 1) {
//			printf("-----%d\n", lastBuff[last_i]);
//		}
	}
}

/*  Detect right channel ready interrupt and do:
 *  Collect data,
 *  store in buffer,
 *  instantly play back.
 */
static void handle_rightready_interrupt_test(void* context, alt_u32 id) {
	 volatile int* rightreadyptr = (volatile int *)context;
	 *rightreadyptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(RIGHTREADY_BASE);
	 IOWR_ALTERA_AVALON_PIO_EDGE_CAP(RIGHTREADY_BASE, 0);
	 /*******Read, playback, store data*******/
	 rightChannel = IORD_ALTERA_AVALON_PIO_DATA(RIGHTDATA_BASE);
	 IOWR_ALTERA_AVALON_PIO_DATA(RIGHTSENDDATA_BASE, rightChannel);
	 rightChannelData[rightCount] = rightChannel;
	 rightCount = (rightCount+1) % BUFFERSIZE;
	 /****************************************/
}


#endif /* YOURISR_H_ */
