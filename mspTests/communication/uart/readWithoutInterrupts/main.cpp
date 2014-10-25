/* 
 * Print all printable characters on usb channel using UART mode of USCI module
 * the print is based in busy wait to transfer characters
 */
  
#include "msp430g2553.h"
  
  
main(void)
  
{
	WDTCTL = WDTPW + WDTHOLD; // Stop WDT

	// Set DCO to 1MHz
	BCSCTL1 = CALBC1_1MHZ; 
	DCOCTL = CALDCO_1MHZ; 

	/* Configure hardware UART */

	UCA0CTL1 |= UCSWRST; // put USCI in reset mode

	// P1.1 = RXD, P1.2=TXD
	P1SEL = BIT1 + BIT2 ;
	P1SEL2 = BIT1 + BIT2 ;

	UCA0CTL1 |= UCSSEL_2; // Use SMCLK

	// Set baud rate to 9600 with 1MHz clock (Data Sheet 15.3.13) 1MHz / 104
	UCA0BR0 = 104;
	UCA0BR1 = 0;

	UCA0MCTL = UCBRS0; // Modulation UCBRSx = 1, correction for the fractional part ( the rest, 9600 - 1MHz / 104)
	UCA0CTL1 &= ~UCSWRST; // put USCI in operad mode (removing reset mode)

	/*End configuration hardware UART */


	//toogle led at every input t
	P1DIR |= BIT0;
	P1OUT &= ~BIT0;
	while (true) { 
		while(!(IFG2 & UCA0RXIFG));
		if ( UCA0RXBUF == 't' )
			P1OUT ^= BIT0;
	}
}
