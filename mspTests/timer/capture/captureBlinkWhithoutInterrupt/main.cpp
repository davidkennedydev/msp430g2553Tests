/* Turn led on if expected pulse is detected
 * generate pwm with the expected pulse and detect this
 * expects 2^16/2=32768 cicles in detection
 * this works on pin 1 of port 1 (commun used for uart comunication)
 * REMEMBER, remove the RX jumper of uart correction in lauchpad, before test
 */

#include <msp/msp430g2553.h>

main() {
	WDTCTL = WDTPW + WDTHOLD; //stop watchdog

	//configure system master clock of 1MHz
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;

	//configure pin 1 of port 1 who capture input
	P1SEL |= BIT1;

	TA0CTL = //timer A0 Control Register
		TASSEL_2 // use who source system master clock
		+ ID_3 // divide source by 8
		+ MC_2; // Continuous mode (count 0 to 0xFFFF)

	TA0CCTL0 = CM_3 //capture on both rising and falling edges
		+ CCIS_0 // input by pin 1 of port 1
		//+ SCS // capture source sincronized
		+ CAP; // capture mode


	//configure pin 1 port 2 who compare output
	P2DIR |= BIT1;
	P2SEL |= BIT1;

	TA1CTL =
		TASSEL_2 // use who source system master clock
		+ ID_3 // divide source by 8
		+ MC_1; // Up mode (count 0 to TAxCCR0)
	
	TA1CCR0 = 0xffff; //periode max value (2^16)
	TA1CCR1 = TA1CCR0 / 2; // 50% duty cycle (2^16/2)

	TA1CCTL1 = OUTMOD_3;
	

	//configure led 1 (pin 0 of port 1) to output
	P1DIR |= BIT0;

	P1OUT &= ~BIT0; // turn off led 1

	unsigned int expectedInterval = TA1CCR1;
	float ERROR = .0; // percentage of error, NOTE this work at 0% error

	while (true) {
		while (!(TA0CCTL0 & CCIFG)); // wait interrupt
		unsigned int interval = TA0CCR0; // get the rising edge moment

		/* WARNING this is essential,
		 * when you don't use interruptions you need clear interruption flag (CCIFG) manualy without exceptions,
		 * don't make this in this program result in a time interval fixed in 0, because this stay in pendent interruption
		 */
		TA0CCTL0 &= ~CCIFG; //clear interrupt flag, 

		while (!(TA0CCTL0 & CCIFG)); // wait interrupt
		interval = TA0CCR0 - interval; // get the falling edge and calculate the interval
		TA0CCTL0 &= ~CCIFG; //clear interrupt flag

		// remain led 1 on, if the interval is the same than expected with a margin error of plus or minus
		if (interval >= expectedInterval * (1 - ERROR) && interval <= expectedInterval * (1 + ERROR))
			P1OUT |= BIT0; //turn on led 1
		else
			P1OUT &= ~BIT0; //turn off led 1
	}
}
