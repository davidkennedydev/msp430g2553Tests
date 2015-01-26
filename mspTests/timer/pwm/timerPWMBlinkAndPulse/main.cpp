/* Generate a PWM with timer A
 * the pin 1.2 will falling with TACCR1 and rising with TACCR0, (reset/set)
 * therefore TACCR0 define the period and TACCR1 the uptime
 *
*/

#include <msp/msp430g2553.h>

#define TACCTLx TACCTL1 //timer comparator 1

main() {
	const int DUTY = 50; // %

	//led pin configuration
	P1DIR |= BIT2 | BIT0;
	P1SEL |= BIT2;

	//DCO configuration
	DCOCTL = DCO1 | DCO0; // divider (divide nominal clock) and modulator (takes a clock between the actual nominal clock and the next)
	BCSCTL1 = RSEL1; //nominal clock
	// 150 kHz to cpu system clock

	//timer Capture/Compare configuration
	TACCR0 = 0xffff; //PWM period of 1s
	TACCR1 = TACCR0 / (100.f / DUTY); //count limit (16 bits)
	TACCTL1 = OUTMOD_7; //select outputmod reset/set -----\_____

	//timer A configuration
	TACTL = TASSEL_2 | ID_2 | MC_1; //control register

	while(true)
		if (TACCTL1 & BIT3)
			P1OUT |= BIT0;
		else
			P1OUT &= ~BIT0;
			
}

