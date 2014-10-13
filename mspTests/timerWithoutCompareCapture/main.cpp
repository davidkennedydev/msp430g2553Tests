/* Blink led 0 at every second (a second on, a second off)
 * using timer A interuption and digital IO
 * Was reduced DCO clock to generate a clock of 150 kHz
 * then the timer A apply four factor division take DCO how input clock
 * generating a clock 37500 Hz, counting 37500 times we have a periode of 1 second.
 * The clock reducing is necessary because timer A have a size of 16 bits,
 * i.e. max 2^16 = 65536 counts
 *
 * The correct way to do this is using a external low frequency souce clock
 * and turn CPU to low power consuption, but this involves a corect use of DMA
 * to don't turn on CPU just to write the samples of the sensors to memory
 * from IO registers.
*/

#include <msp/msp430g2553.h>

main() {
	//led pin configuration
	P1SEL &= ~BIT0;
	P1DIR |= BIT0;	

	//DCO configuration
	DCOCTL = DCO1 | DCO0; // divider (divide nominal clock) and modulator (takes a clock between the actual nominal clock and the next)
	BCSCTL1 = RSEL1; //nominal clock
	// 150 kHz to cpu system clock

	//timer Capture/Compare configuration
	TACCR0 = 37500; //count limit (16 bits)
	TACCTL0 = CCIE; //enable capture compare interrupt

	//timer A configuration
	TACTL = TASSEL_2 | ID_0 | MC_1 | TAIE | ID_2; //control register
	// 150 kHz / 4 = 37500 Hz to timer

	__enable_interrupt();
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A() {
	P1OUT ^= BIT0;
}	

