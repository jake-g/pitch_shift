/*
 * Jake and Jisoo
 */

/* This is the main file that contains the main function and the
 * main while(1) loop.
 * Once you understand our code, please feel free to modify them
 * to meet your project or homework requirement
 */

#define PI 3.141592

/*Major header file includes all system function you need*/
#include "system_init.h"

/*The header file where you store your ISR*/
#include "yourISR.h"

/************************************************************************/
/********************Global Variables for your system********************/
/************************************************************************/

//Value for interrupt ID
alt_u32 switch0_id = SWITCH0_IRQ;
alt_u32 switch1_id = SWITCH1_IRQ;
alt_u32 key0_id = KEY0_IRQ;
alt_u32 key1_id = KEY1_IRQ;
alt_u32 key2_id = KEY2_IRQ;
alt_u32 key3_id = KEY3_IRQ;
alt_u32 leftready_id = LEFTREADY_IRQ;
alt_u32 rightready_id = RIGHTREADY_IRQ;
alt_u32 uart_id = UART_IRQ;

/*Use for ISR registration*/
volatile int switch0 = 0;
volatile int switch1 = 0;
volatile int key0 = 0;
volatile int key1 = 0;
volatile int key2 = 0;
volatile int key3 = 0;
volatile int leftready = 0;
volatile int rightready = 0;


//AIC default setting value   L-in     R-in    L-H-Vo  R-H-Vo  Ana-Cl  Dig-Cl  Power   Dig-It  SR-Clt  Dig-Act
//                                                             0x0014->micIn
unsigned int aic23_demo[10] = {0x0017, 0x0017, 0x01f9, 0x01f9, 0x0012, 0x0000, 0x0000, 0x0042, 0x0001, 0x0001};

//leftChannel and rightChannel are the instant values of the value reading from ADC
int leftChannel = 0;
int rightChannel = 0;
int convIndex = 0;

//Default ADC Sampling frequency = 8k
int sampleFrequency = 0x000C;

//Buffer that store values from different channels
alt_16 leftChannelData[BUFFERSIZE];
alt_16 rightChannelData[BUFFERSIZE];
int convResultBuffer[CONVBUFFSIZE];

/*uart-global
 * RxHead: integer indicator tells you the index of where the
 * newest char data you received from host computer
 *
 * rx_buffer-> A ring buffer to collect uart data sent by host computer
 * */
alt_16 datatest[UART_BUFFER_SIZE];
unsigned short RxHead=0;
unsigned char rx_buffer[RX_BUFFER_SIZE];

/*Channel indicators: indicate the index of the most recent collected data*/
int leftCount = 0;
int lefttestCount = 0;
int rightCount = 0;
int calBuffersize = BUFFERSIZE - 1;
int leftBufferFull = 0;

/*Send flag: 1->ok, now send data to the host
 *           0->ok, will not send any data to the host
 *Recv flag: 1->ok, now check and store data from the host
 *           0->ok, will not receive any data from the host
 */
int uartStartSendFlag = 0;
int uartStartRecvFlag = 0;

/*for uart receive purpose*/
int sr = 0;

/*0->do not update sampling frequency*/
/*1->ok, update sampling frequency to AIC23*/
int setFreqFlag = 0;

/*uart object*/
int uart;

/*System initialization function. Should be called before your while(1)*/
void system_initialization(){
	/*Hard-code to 1 right here, you can use ISR
	 *to change the value by yourself
	*/
	uartStartRecvFlag = 1;

	 /*Open Uart port and ready to transmit and receive*/
	 uart = open(UART_NAME, O_ACCMODE);
	 if(!uart){
		 printf("failed to open uart\n");
		 //return 0;
	 } else {
		 printf("Uart ready!\n");
	 }

	 //Interrupts Registrations
	 alt_irq_register(switch0_id, (void *)&switch0, handle_switch0_interrupt);
	 alt_irq_register(switch1_id, (void *)&switch1, handle_switch1_interrupt);
	 alt_irq_register(key0_id, (void *)&key0, handle_key0_interrupt);
	 alt_irq_register(key1_id, (void *)&key1, handle_key1_interrupt);
	 alt_irq_register(key2_id, (void *)&key2, handle_key2_interrupt);
	 alt_irq_register(key3_id, (void *)&key3, handle_key3_interrupt);
	 alt_irq_register(leftready_id, (void *)&leftready, handle_leftready_interrupt_test);
	 alt_irq_register(rightready_id, (void *)&rightready, handle_rightready_interrupt_test);

	 /*Interrupt enable -> mask to enable it*/
	 IOWR_ALTERA_AVALON_PIO_IRQ_MASK(SWITCH0_BASE, 1);
	 IOWR_ALTERA_AVALON_PIO_IRQ_MASK(SWITCH1_BASE, 1);
	 IOWR_ALTERA_AVALON_PIO_IRQ_MASK(KEY0_BASE, 1);
	 IOWR_ALTERA_AVALON_PIO_IRQ_MASK(KEY1_BASE, 1);
	 IOWR_ALTERA_AVALON_PIO_IRQ_MASK(KEY2_BASE, 1);
	 IOWR_ALTERA_AVALON_PIO_IRQ_MASK(KEY3_BASE, 1);
	 IOWR_ALTERA_AVALON_PIO_IRQ_MASK(LEFTREADY_BASE, 1);
	 IOWR_ALTERA_AVALON_PIO_IRQ_MASK(RIGHTREADY_BASE, 1);

	 /*Reset edge capture bit*/
	 IOWR_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH0_BASE, 0);
	 IOWR_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH1_BASE, 0);
	 IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY0_BASE, 0);
	 IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY1_BASE, 0);
	 IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY2_BASE, 0);
	 IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY3_BASE, 0);
	 IOWR_ALTERA_AVALON_PIO_EDGE_CAP(LEFTREADY_BASE, 0);
	 IOWR_ALTERA_AVALON_PIO_EDGE_CAP(RIGHTREADY_BASE, 0);

	 /*turn off all LEDs*/
	 IOWR_ALTERA_AVALON_PIO_DATA(LED_BASE, 0x00);

	 /*initialize SPI transmission*/
	 IOWR_ALTERA_AVALON_PIO_DATA(CS_BASE, 1); // ~CS low
	 IOWR_ALTERA_AVALON_PIO_DATA(SCLK_BASE, 0); // Initialize SCLK to high
}

/*
Based on the example, Sine8_LED.c Sine generation with DIP switch control, given in the
lecture note 2, generate an 800Hz sine wave with an 8KHz sampling rate (10 samples per period) and
output to the LINEOUT jack of the AIC23 daughter card. Use an oscilloscope attached to the LINEOUT jack
to verify. Also take advantage of the data exporting via UART (see the example in Lecture Note 1 about
transferring a chunk of data to Matlab via UART), use the appropriate Matlab command to plot the 256
most recent output samples in the time domain, as well as the FFT magnitudes of these 256 samples.
 *
 * */

int main(void) {
	system_initialization();
	// set frequency
	sampleFrequency = 0x000C; //8k
	//sampleFrequency = 0x0019; //32k
//	sampleFrequency = 0x0023; //44.1k
	//sampleFrequency = 0x0001; //48k
	aic23_demo[4] = 0x0014;
	aic23_demo[8] = sampleFrequency;
	AIC23_demo();


	memset(PING, 0, BUFF_SIZE);
	memset(PONG, 0, BUFF_SIZE);
	memset(lastBuff, 0, BUFF_SIZE);


	printf("float = %d, int = %d\n", sizeof(float), sizeof(int));
	while(1){
		if (uartStartSendFlag) {
			printf("UART SENT\n");
			uartStartSendFlag = 0;
		}
		if (input_ready) {
			PitchShift(pitch_factor, processBuffer, processBuffer);
			input_ready = 0;
		}
	}

	/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
	/*!!!!!!!YOUR CODE SHOULD NEVER REACH HERE AND BELOW!!!!!!!*/
	/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
	return 0;
}
