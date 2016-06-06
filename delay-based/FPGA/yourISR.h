/*
 * Jake and Jisoo
 */

/*
 * yourISR.h
 *
 */

#ifndef YOURISR_H_
#define YOURISR_H_

#include <string.h>
#include "PitchShift.h"
#include "echo.h"
#include "melody.h"
#include "system_init.h"
#include <math.h>

#define UART_BUFFER_SIZE 8000

void waitFor(unsigned int n) {
  printf("Pausing %d iterations...", n);
  int i;
  for (i = 0; i < n; i++)
    ;
  printf("Finished\n");
}
// ------------------------------------------------------
// Buffers
int PONG[BUFF_SIZE];              // Transmit PING buffer
int PING[BUFF_SIZE];              // Transmit PONG buffer
int lastBuff[BUFF_SIZE];          // tmp buffer
int UARTInput[UART_BUFFER_SIZE];  // UART buff

// Pointers
int* playAndFillBuffer = NULL;
int* processBuffer = NULL;

// Settings
// float pitch_factor = 1; // this is moved to echo.h
int semitone = 1;

// Counters
int sampleIndex = 0;
int last_i = 0;
int iii = 0;

// Flags
int input_ready = 0;
short ptrStatus = 0;
int semitoneFlag = 0;
int melodyFlag = 0;

// Varaibles to handle switch toggle safely
short prevSw0 = 0;
short prevSw1 = 0;
short prevSw2 = 0;
short prevSw3 = 0;
short prevSw4 = 0;
short prevSw5 = 0;
short prevSw7 = 0;

short currSw0 = 0;
short currSw1 = 0;
short currSw2 = 0;
short currSw3 = 0;
short currSw4 = 0;
short currSw5 = 0;
short currSw7 = 0;

// Switch configuration: 1 -> on, 0 -> off
// - swtich 1 through 4 state: later enabled switch overrides others
// - switch1: melody mode
// - switch2: pitch echo mode
// - switch3: echo mode
// - switch4: loop mode
int switchMask1_4 = 0;

short rightToggle = 1;
// ------------------------------------------------------

// AIC23
extern unsigned int aic23_demo[10];

// Value for interrupt ID
extern alt_u32 switch0_id;
extern alt_u32 switch1_id;
extern alt_u32 switch2_id;
extern alt_u32 switch3_id;
extern alt_u32 switch4_id;
extern alt_u32 switch5_id;
extern alt_u32 switch7_id;
extern alt_u32 key0_id;
extern alt_u32 key1_id;
extern alt_u32 key2_id;
extern alt_u32 key3_id;
extern alt_u32 leftready_id;
extern alt_u32 rightready_id;
extern alt_u32 uart_id;

// leftChannel and rightChannel are the instant values of the value reading from
// ADC
extern int leftChannel;
extern int rightChannel;
extern int convResultBufferSize;
extern int convIndex;

// Default ADC Sampling frequency = 8k
extern int sampleFrequency;

// Buffer that store values from different channels
extern alt_16 leftChannelData[BUFFERSIZE];
extern alt_16 rightChannelData[BUFFERSIZE];
extern int convResultBuffer[CONVBUFFSIZE];

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

/*uart object*/
extern int uart;

// TOGGLE SEMITONE
static void handle_switch0_interrupt(void* context, alt_u32 id) {
  volatile int* switch0ptr = (volatile int*)context;
  *switch0ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH0_BASE);

  /* Write to the edge capture register to reset it. */
  IOWR_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH0_BASE, 0);

  /*Perform Jobs*/
  currSw0 = IORD_ALTERA_AVALON_PIO_DATA(SWITCH0_BASE);
  if (prevSw0 != currSw0) {
    semitoneFlag = currSw0;
    gain = 0.6;
    prevSw0 = currSw0;
    printf("Semitone Toggle : %d\n", semitoneFlag);
  }
}

// TOGGLE MELODY
static void handle_switch1_interrupt(void* context, alt_u32 id) {
  volatile int* switch1ptr = (volatile int*)context;
  *switch1ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH1_BASE);

  /* Write to the edge capture register to reset it. */
  IOWR_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH1_BASE, 0);

  /*Perform Jobs*/
  currSw1 = IORD_ALTERA_AVALON_PIO_DATA(SWITCH1_BASE);
  if (prevSw1 != currSw1) {
    melodyFlag = currSw1;
    gain = 0.6;
    prevSw1 = currSw1;
    // 0b0001
    switchMask1_4 = melodyFlag;
    printf("Melody Toggle : %d\n", switchMask1_4);
  }
}

static void handle_switch2_interrupt(void* context, alt_u32 id) {
  volatile int* switch2ptr = (volatile int*)context;
  *switch2ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH2_BASE);

  /* Write to the edge capture register to reset it. */
  IOWR_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH2_BASE, 0);

  /*Perform Jobs*/
  currSw2 = IORD_ALTERA_AVALON_PIO_DATA(SWITCH2_BASE);
  if (prevSw2 != currSw2) {
    gain = 0.6;
    prevSw2 = currSw2;
    // 0b0010
    switchMask1_4 = (currSw2 << 1);
    printf("Pitch Decay Toggle : %d\n", switchMask1_4);
  }
}

static void handle_switch3_interrupt(void* context, alt_u32 id) {
  volatile int* switch3ptr = (volatile int*)context;
  *switch3ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH3_BASE);

  /* Write to the edge capture register to reset it. */
  IOWR_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH3_BASE, 0);

  /*Perform Jobs*/
  currSw3 = IORD_ALTERA_AVALON_PIO_DATA(SWITCH3_BASE);
  if (prevSw3 != currSw3) {
    gain = 0.6;
    prevSw3 = currSw3;
    // 0b0100
    switchMask1_4 = (currSw3 << 2);
    printf("Echo Toggle : %d\n", switchMask1_4);
  }
}

static void handle_switch4_interrupt(void* context, alt_u32 id) {
  volatile int* switch4ptr = (volatile int*)context;
  *switch4ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH4_BASE);

  /* Write to the edge capture register to reset it. */
  IOWR_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH4_BASE, 0);

  /*Perform Jobs*/
  currSw4 = IORD_ALTERA_AVALON_PIO_DATA(SWITCH4_BASE);
  if (prevSw4 != currSw4) {
    gain = 1;
    prevSw4 = currSw4;
    // 0b1000
    switchMask1_4 = (currSw4 << 3);
    printf("Loop Toggle : %d\n", switchMask1_4);
  }
}

static void handle_switch5_interrupt(void* context, alt_u32 id) {
  volatile int* switch5ptr = (volatile int*)context;
  *switch5ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH5_BASE);

  /* Write to the edge capture register to reset it. */
  IOWR_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH5_BASE, 0);

  /*Perform Jobs*/
  currSw5 = IORD_ALTERA_AVALON_PIO_DATA(SWITCH5_BASE);
  if (prevSw5 != currSw5) {
    printf("Dry Sound to Right Ch Toggle : %d\n",
           IORD_ALTERA_AVALON_PIO_DATA(SWITCH5_BASE));
    rightToggle = currSw5;
    prevSw5 = currSw5;
  }
}

static void handle_switch7_interrupt(void* context, alt_u32 id) {
  volatile int* switch7ptr = (volatile int*)context;
  *switch7ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH7_BASE);

  /* Write to the edge capture register to reset it. */
  IOWR_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH7_BASE, 0);

  /*Perform Jobs*/
  currSw7 = IORD_ALTERA_AVALON_PIO_DATA(SWITCH7_BASE);
  if (prevSw7 != currSw7) {
    if (currSw7) {
      // MIC IN
      aic23_demo[4] = 0x0014;
      printf("MIC IN enabled\n");
    } else {
      // LINE IN
      aic23_demo[4] = 0x0012;
      printf("LINE IN enabled\n");
    }
    AIC23_demo();
    prevSw7 = currSw7;
  }
}

// RESET PITCH, SEMITONE, SAMPLE DELAY, PITCH RATE CHANGE
static void handle_key0_interrupt(void* context, alt_u32 id) {
  volatile int* key0ptr = (volatile int*)context;
  *key0ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(KEY0_BASE);

  /* Write to the edge capture register to reset it. */
  IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY0_BASE, 0);

  // reset
  pitch_factor = 1;
  semitone = 1;
  sampleDelay = MIN_DELAY;
  pitchChangeRate = MIN_PITCH_RATE;
  printf("Pitch Reset, delay reset\n");
}

// PITCH DOWN OR DELAY DOWN
static void handle_key1_interrupt(void* context, alt_u32 id) {
  volatile int* key1ptr = (volatile int*)context;
  *key1ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(KEY1_BASE);

  /* Write to the edge capture register to reset it. */
  IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY1_BASE, 0);

  if (switchMask1_4 == 0b0010) {
    changePitchRate(-1);
  } else if (switchMask1_4 == 0b0100) {
    changeDelay(-1);
  } else {
    if (pitch_factor <= 0) {
      printf("pitch_factor cannot be lower than 0\n");
    } else {
      if (semitoneFlag == 1) {
        semitone--;
        printf("Current Semitone : %d, ", semitone);
        pitch_factor = pow(2, semitone / 12.0);
      } else {
        pitch_factor = pitch_factor - 0.1;
      }
      printf("Pitch decreased to : %.3f\n", pitch_factor);
    }
  }
}

// PITCH UP OR DELAY UP
static void handle_key2_interrupt(void* context, alt_u32 id) {
  volatile int* key2ptr = (volatile int*)context;
  *key2ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(KEY2_BASE);

  /* Write to the edge capture register to reset it. */
  IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY2_BASE, 0);

  if (switchMask1_4 == 0b0010) {
    changePitchRate(1);
  } else if (switchMask1_4 == 0b0100) {
    changeDelay(1);
  } else {
    if (semitoneFlag == 1) {
      semitone++;
      printf("Current Semitone : %d\t", semitone);
      pitch_factor = pow(2, semitone / 12.0);
    } else {
      pitch_factor = pitch_factor + 0.1;
    }
    printf("Pitch increased to : %.3f\n", pitch_factor);
  }
}

// UART send
static void handle_key3_interrupt(void* context, alt_u32 id) {
  volatile int* key3ptr = (volatile int*)context;
  *key3ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(KEY3_BASE);

  /* Write to the edge capture register to reset it. */
  IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY3_BASE, 0);

  uartStartSendFlag = 1;
}

/*  Detect left channel ready interrupt and do:
 *  Collect data,
 *  store in buffer,
 *  instantly play back.
 *
 */
int unsigned2signed(alt_16 unsign) {
  int result;
  if (unsign > 32767)
    result = unsign - 65536;
  else
    result = unsign;
  return result;
}

alt_16 signed2unsigned(int sign) {
  alt_16 result;
  if (sign < 0)
    result = sign + 65536;
  else
    result = sign;
  return result;
}

static void handle_leftready_interrupt_test(void* context, alt_u32 id) {
  volatile int* leftreadyptr = (volatile int*)context;
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

    // -------------- ECHO ------------------
    if (switchMask1_4 == 0b0010 && delayIndex % 200 == 0) {
      // pitch changing echo changes pitch every N samples
      changePitch();
    }
    int x_t = getEchoSample() + playAndFillBuffer[sampleIndex];
    int prev = delayedBuffer[delayIndex];
    delayedBuffer[delayIndex] = x_t;
    delayIndex = (delayIndex + 1) % DELAY_BUFFER_LENGTH;
    // --------------------------------------

    // play and fill operation:
    // - decide which sample to play: choose from delay buffer and
    // playAndFillBuffer
    // - switch1 0b0001 to control melody mode
    // - switch2 0b0010 to control pitch echo mode
    // - switch3 0b0100 to control echo mode
    // - switch4 0b1000 to control loop mode
    if ((switchMask1_4 == 0b0010) || (switchMask1_4 == 0b0100)) {
      IOWR_ALTERA_AVALON_PIO_DATA(LEFTSENDDATA_BASE, x_t);
    } else if (switchMask1_4 == 0b1000) {
      IOWR_ALTERA_AVALON_PIO_DATA(LEFTSENDDATA_BASE, prev + leftChannel);
    } else {
      IOWR_ALTERA_AVALON_PIO_DATA(LEFTSENDDATA_BASE,
                                  melGain * playAndFillBuffer[sampleIndex]);
    }
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
    // Play back last buffer while waiting for processing
    IOWR_ALTERA_AVALON_PIO_DATA(LEFTSENDDATA_BASE, lastBuff[last_i]);
    last_i = (last_i + 1) % BUFF_SIZE;
  }
}

/*  Detect right channel ready interrupt and do:
 *  Collect data,
 *  store in buffer,
 *  instantly play back.
 */
static void handle_rightready_interrupt_test(void* context, alt_u32 id) {
  volatile int* rightreadyptr = (volatile int*)context;
  *rightreadyptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(RIGHTREADY_BASE);
  IOWR_ALTERA_AVALON_PIO_EDGE_CAP(RIGHTREADY_BASE, 0);
  /*******Read, playback, store data*******/
  rightChannel = IORD_ALTERA_AVALON_PIO_DATA(RIGHTDATA_BASE);
  IOWR_ALTERA_AVALON_PIO_DATA(RIGHTSENDDATA_BASE, rightToggle * rightChannel);
  UARTInput[rightCount] = rightChannel;
  rightCount = (rightCount + 1) % UART_BUFFER_SIZE;
  /****************************************/
}

#endif /* YOURISR_H_ */
