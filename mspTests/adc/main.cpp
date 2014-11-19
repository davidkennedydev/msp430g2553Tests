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
	ADC10CTL0 = SREF_0 	// reference, Vr+ = Vcc and Vr- = Vss
		| ADC10SHT_0 	// sample and hold time, 4 * ADC10CLKs
		| REFON 	// enable internal reference, default 1.5V
	;
	ADC10CTL1 = INCH_0 	// input channel A0
		| SHS_0 	// sample and hold source, ADC10SC bit
		| ADC10DIV_0 	// ADC10 clock divider, /1
		| ADC10SSEL_0	// ADC10 clock source, ADC10OSC
		| CONSEQ_0	// Conversion sequence mode, Single channel single conversion
	;
	ADC10AE0 |= BIT0; 	// configure pin 1.0 (A0) to analog input
	ADC10CTL0 |= ADC10ON;	// turn on ADC10
	ADC10CTL0 |= ENC; 	// enable conversion
	

	//toogle led at every input t
	const unsigned int EXPECTED_VOLTAGE 		= 116; // mV, expected tension on pin 1.0 (A0)
	const float VOLTAGE_REFERENCE 			= 1500; // mV, internal default voltage reference
	const unsigned int QUANTITY_OF_REPRESENTATIONS 		= 0x400, // u, (2^10), representations on ADC
		VOLTAGE_BY_UNIT_OF_ADC = VOLTAGE_REFERENCE / QUANTITY_OF_REPRESENTATIONS, // mV/u
		EXPECTED_VOLTAGE_ADC10_VALUE = EXPECTED_VOLTAGE / VOLTAGE_BY_UNIT_OF_ADC;

	const float ERROR = 0.20; // error at plus or minus


	P1DIR = BIT6; // configure pin 1.6 (led2 pin) to output
	P1OUT &= ~BIT6; //turn off led2
	while (true) { 
		ADC10CTL0 |= ADC10SC; // start conversion

		while(!(ADC10CTL0 & ADC10IFG));
		ADC10CTL0 &= ~ADC10IFG;

		// compare capture with expected tension, considering a margin of error 
		if (ADC10MEM > EXPECTED_VOLTAGE_ADC10_VALUE * (1 - ERROR) // botton limit
		&& ADC10MEM < EXPECTED_VOLTAGE_ADC10_VALUE * (1 + ERROR)) // up limit 
			P1OUT |= BIT6; //turn on led2
		else
			P1OUT &= ~BIT6; //turn off led2
	}
}
