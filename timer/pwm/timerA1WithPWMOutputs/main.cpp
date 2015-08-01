/* Generate a PWM with timer A1
 * the pin 2.1 will falling with TA1CCR1 and rising with TA1CCR0, (reset/set)
 * therefore TACCR0 define the period and TACCR1 the uptime
 *
*/

#include <msp/msp430g2553.h>


main() {
	const int DUTY = 50; // %

	//led pin configuration
	P2DIR |= BIT1 + BIT2;
	P2SEL |= BIT1 + BIT2;

	//DCO configuration
	DCOCTL = DCO1 + DCO0; // divider (divide nominal clock) and modulator (takes a clock between the actual nominal clock and the next)
	BCSCTL1 = RSEL1; //nominal clock
	// 150 kHz to cpu system clock

	//timer Capture/Compare configuration
	TA1CCR0 = 37500 / 1000; //PWM period of 1ms
	TA1CCR1 = TA1CCR0 / (100.f / DUTY); //count limit (16 bits)
	TA1CCTL1 = OUTMOD_7; //select outputmod reset/set -----\_____

	//timer A configuration
	TA1CTL = TASSEL_2 + ID_2 + MC_1; //control register
}

