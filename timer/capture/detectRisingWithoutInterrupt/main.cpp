/* Turn led on if expected pulse is detected
 * expects 2^16/2=32768 cicles
 * generate pwm with the expected pulse and detect this
 */

#include <msp/msp430.h>
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

	TA0CCR0 = 0; //clear capture register

	TA0CCTL0 = CM_1 //capture on rising
		+ CCIS_0 // input by pin 1 of port 1
		+ SCS // capture source sincronized
		+ CAP; // capture mode

	//configure led 1 (pin 0 of port 1) to output
	P1DIR |= BIT0;

	P1OUT &= ~BIT0; // turn off led 1
	while (!(TA0CCTL0 & CCIFG)); // wait interrupt
	P1OUT |= BIT0; // turn off led 1
	TA0CCTL0 &= ~CCIFG;
}



	
