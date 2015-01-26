/* Generate a PWM with timer A
 * the tension on pin 1.2 variate from 0 (0%) to 3.6 (100%) using duty cicle
 *
*/

#include <msp/msp430g2553.h>

#define TACCTLx TACCTL1

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
	TACCR0 = 0xffff/1000; //PWM period of 1ms
	TACCR1 = TACCR0 / (100.f / DUTY); //count limit (16 bits)
	TACCTLx = OUTMOD_7; //select outputmod reset/set  -----\_____

	//timer A configuration
	TACTL = TASSEL_2 | ID_2 | MC_1; //control register
}

