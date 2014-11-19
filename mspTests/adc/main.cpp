/* 
 * turn on led2 if expectd voltage match on A0 (pin 1.0)
 *
 * REMEMBER: unplug led1 jumper
 */
  
#include "msp/msp430g2553.h"
  
main(void)
  
{
	WDTCTL = WDTPW + WDTHOLD; // Stop WDT

	// Set DCO to 1MHz
	BCSCTL1 = CALBC1_1MHZ; 
	DCOCTL = CALDCO_1MHZ; 

	//Setup ADC
	ADC10CTL0 = SREF0 	// reference, Vr+ = Vcc and Vr- = Vss
		| ADC10SHT0 	// sample and hold time, 4 * ADC10CLKs
		| REFON 	// enable internal reference, default 1.5V
	;
	ADC10CTL1 = INCH0 	// input channel A0
		| SHS0 		// sample and hold source, ADC10SC bit
		| ADC10DIV0 	// ADC10 clock divider, /1
		| ADC10SSEL0	// ADC10 clock source, ADC10OSC
		| CONSEQ0	// Conversion sequence mode, Single channel single conversion
	;
	ADC10CTL0 |= ADC10ON;	// turn on ADC10
	ADC10CTL0 |= ENC; 	// enable conversion


	//toogle led at every input t
	const unsigned int EXPECTED_TENSION = 0;
	P1DIR |= BIT6;
	P1OUT &= ~BIT6;
	while (true) { 
		ADC10CTL0 |= ADC10SC;	// start conversion
		while(!(ADC10CTL0 & ADC10IFG));
		if (ADC10MEM == EXPECTED_TENSION) // compare captured with expected
			P1OUT |= BIT6;
		else
			P1OUT &= ~BIT6;
	}
}
