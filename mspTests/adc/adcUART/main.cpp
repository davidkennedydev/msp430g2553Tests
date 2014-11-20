/* 
 * turn on led2 if expectd voltage match on A3 (pin 1.3)
 *
 * REMEMBER: unplug led1 jumper
 */
  
#include "msp/msp430g2553.h"
  
void putc(char);
void print (unsigned int);

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
	ADC10CTL1 = INCH_3 	// input channel A3
		| SHS_0 	// sample and hold source, ADC10SC bit
		| ADC10DIV_0 	// ADC10 clock divider, /1
		| ADC10SSEL_0	// ADC10 clock source, ADC10OSC
		| CONSEQ_0	// Conversion sequence mode, Single channel single conversion
	;
	ADC10AE0 |= BIT3; 	// configure pin 1.3 (A3) to analog input
	ADC10CTL0 |= ADC10ON;	// turn on ADC10
	ADC10CTL0 |= ENC; 	// enable conversion
	
	
	/* Configure hardware UART */

	UCA0CTL1 |= UCSWRST; // put USCI in reset mode

	// P1.1 = RXD, P1.2=TXD
	P1SEL |= BIT1 + BIT2 ;
	P1SEL2 |= BIT1 + BIT2 ;

	UCA0CTL1 |= UCSSEL_2; // Use SMCLK

	// Set baud rate to 9600 with 1MHz clock (Data Sheet 15.3.13) 1MHz / 104
	UCA0BR0 = 104;
	UCA0BR1 = 0;

	UCA0MCTL = UCBRS0; // Modulation UCBRSx = 1, correction for the fractional part ( the rest, 9600 - 1MHz / 104)
	UCA0CTL1 &= ~UCSWRST; // put USCI in operad mode (removing reset mode)

	
	putc('>'), putc('\r'), putc('\n');



	//toogle led at every input t
	const unsigned int EXPECTED_VOLTAGE 		= 116; // mV, expected tension on pin 1.3 (A3)
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

		const unsigned CAPTURED_VOLTAGE = ADC10MEM * VOLTAGE_BY_UNIT_OF_ADC;
		print(CAPTURED_VOLTAGE);
		putc(' ');
		putc(' ');

		// compare capture with expected tension, considering a margin of error 
		if (CAPTURED_VOLTAGE > EXPECTED_VOLTAGE * (1 - ERROR) // botton limit
		&& CAPTURED_VOLTAGE < EXPECTED_VOLTAGE * (1 + ERROR)) // up limit 
			putc('o'), putc('k'), putc('\r'), putc('\n');
		else
			putc('\r'), __delay_cycles(65000);
	}
}


void putc(char ch) {
	while(!(IFG2 & UCA0TXIFG));
	UCA0TXBUF = ch;
}

void print (unsigned int value) {
	if (value == 0) { putc('0'); return; }
	if (value < 0) {
		putc('-');
		value *= -1;
	}
	unsigned int size;
	char buffer[20];
	for (size = 0; value; ++size, value /= 10)
		buffer[size] = value % 10;
	while (size)
		putc(buffer[--size] + '0');
}

