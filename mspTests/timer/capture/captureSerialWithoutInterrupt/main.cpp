/* print measured pulse width value via serial (uart)
 * generate pwm with the expected pulse and detect this
 * expects 2^16/2=32768 cicles in detection
 * this works on pin 1 of port 1 (commun used for uart comunication)
 * REMEMBER, remove the led2 jumper of uart correction in lauchpad, before test
 * generate pwm in pin 6 of port 1 and capture by pin 0 of port 2
 */

#include <msp/msp430g2553.h>

void print (unsigned int);
void putc(char ch);

main() {
	WDTCTL = WDTPW + WDTHOLD; //stop watchdog

	//configure system master clock of 1MHz
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;

	//configure pin 6 port 1 who compare output
	P1DIR |= BIT6;
	P1SEL |= BIT6;

	TA0CTL =
		TASSEL_2 // use who source system master clock
		+ ID_3 // divide source by 8
		+ MC_1; // Up mode (count 0 to TAxCCR0)
	
	TA0CCR0 = 0xffff; //periode max value (2^16)
	TA0CCR1 = TA0CCR0 / 2; // 50% duty cycle (2^16/2)

	TA0CCTL1 = OUTMOD_7; //comparator 1 at timer A0 generate reset/set mode pwm
	

	//configure pin 0 of port 2 who capture input
	P2SEL |= BIT0;

	TA1CTL = //timer A0 Control Register
		TASSEL_2 // use who source system master clock
		+ ID_3 // divide source by 8
		+ MC_2; // Continuous mode (count 0 to 0xFFFF)

	TA1CCTL0 = // comparator 0 at timer A1
		CM_3 //capture on both rising and falling edges
		+ CCIS_0 // input by CCIxA (pin 0 of port 2)
		//+ SCS // capture source sincronized
		+ CAP; // capture mode
	
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

	unsigned int expectedInterval = TA0CCR1;
	float ERROR = .0; // percentage of error, NOTE this work at 0% error

	while (true) {
		while (!(TA1CCTL0 & CCIFG && CCI)); // wait interrupt at rising edge
		unsigned int interval = TA1CCR0; // get the rising edge moment

		/* WARNING this is essential,
		 * when you don't use interruptions you need clear interruption flag (CCIFG) manualy without exceptions,
		 * don't make this in this program result in a time interval fixed in 0, because this stay in pendent interruption
		 */
		TA1CCTL0 &= ~CCIFG; //clear interrupt flag, 

		while (!(TA1CCTL0 & CCIFG)); // wait interrupt
		interval = TA1CCR0 - interval; // get the falling edge and calculate the interval
		TA1CCTL0 &= ~CCIFG; //clear interrupt flag
		
		// print measured width pulse
		print(interval), putc(' ');

		// print ok, if the interval is the same than expected with a margin error of plus or minus
		if (interval >= expectedInterval * (1 - ERROR) && interval <= expectedInterval * (1 + ERROR))
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
	
