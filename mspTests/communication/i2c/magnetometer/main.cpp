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
	
	// write desired slave address to the UCBxI2CSA
	// select size of addres with UCSLA10
	// setting UCTR for transmitter mode
	// setting UCTXSTT to generate START condition
	
	// while have data to be transmitted
	// wait UCBxTXIFG are set, when set the buffer are empty
	// load data to be trasmitted on UCBxTXBUF

	// if want stop transmission
	// setting UCTXSTP, STOP condition is generated

	// if want set slave address or transmitter/reciver mode
	// setting UCTXSTT communicate RESTART

	// if not-acknowledge was recived UCNACKIFG is set
}
