/* 
 * Print all printable characters on usb channel using UART mode of USCI module
 * the print is based in busy wait to transfer characters
 */

#include "msp/msp430g2553.h"
  
void putc(char);
void print (const char[]);
void print (short);
	 
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

	UCA0CTL1 |= UCSSEL_3; // Use SMCLK

	// Set baud rate to 9600 with 1MHz clock (Data Sheet 15.3.13) 1MHz / 104
	UCA0BR0 = 104;
	UCA0BR1 = 0;

	UCA0MCTL = UCBRS0; // Modulation UCBRSx = 1, correction for the fractional part ( the rest, 9600 - 1MHz / 104)
	UCA0CTL1 &= ~UCSWRST; // put USCI in operad mode (removing reset mode)

	/*End configuration hardware UART */
	for (int i = 0; i < 10; ++i) print(i);
	putc('\r'), putc('\n');
	putc('\r'), putc('\n');

	
	P1DIR |= BIT0;
	P1OUT &= ~BIT0;
	

	// Starting I2C

	while (UCB0CTL1 & UCTXSTT);
	
	//configure i2 pins
	P1SEL |= BIT6 | BIT7;
	P1SEL2|= BIT6 | BIT7;

	UCB0CTL1 |= UCSWRST; // disable

	UCB0CTL0 =  		// USCI_B0 control register
		UCMST		// set master mode	
		| UCMODE_3 	// I2C mode select
		| UCSYNC; 	// synchronous


	UCB0CTL1 = 		// UCSI_B0 control register
		UCSSEL_3;	//select clock source, SMCLK
		//| UCSWRST;

	UCB0I2CSA = 0x1e;	// write desired slave address to the UCBxI2CSA, magnetometer address
	UCB0BR0 = 0xff;
	UCB0BR1 = 0xff;

	// select size of addres with UCSLA10, 7 bits default
	
	UCB0CTL1 &= ~UCSWRST; // enable

	// Master Transmitter
	

	const unsigned MESSAGE_SIZE = 2;
	const char 	CONFIGURE_8_AVERANGE_15HZ_POSITIVE_SELFTEST[] 	= { 0x00, 0x71 },
			CONFIGURE_GAIN_5[] 				= { 0x01, 0xa0 },
			CONFIGURE_CONTINUOUS_MEASUREMENT_MODE[] 	= { 0x02, 0x00 },
			CONFIGURE_EXIT_SELF_TEST[]			= { 0x00, 0x70 };
			
				
	UCB0CTL1 |=
		  UCTR		// setting UCTR for transmitter mode
		| UCTXSTT; 	// setting UCTXSTT to generate START condition
	for (int i = 0; i < MESSAGE_SIZE; ++i) { // while have data to be transmitted
		UCB0TXBUF = CONFIGURE_8_AVERANGE_15HZ_POSITIVE_SELFTEST[i]; // load data to be trasmitted on UCBxTXBUF
		while ( !(IFG2 & UCB0TXIFG) ); // wait UCBxTXIFG are set, when set the buffer are empty
	}
	//UCB0CTL1 |= UCTXSTP; // setting UCTXSTP, STOP condition is generated
	//while (UCB0CTL1 & UCTXSTT);

	UCB0CTL1 |=
		  UCTR		// setting UCTR for transmitter mode
		| UCTXSTT; 	// setting UCTXSTT to generate START condition
	for (int i = 0; i < MESSAGE_SIZE; ++i) { // while have data to be transmitted
		UCB0TXBUF = CONFIGURE_GAIN_5[i]; // load data to be trasmitted on UCBxTXBUF
		while ( !(IFG2 & UCB0TXIFG) ); // wait UCBxTXIFG are set, when set the buffer are empty
	}

	UCB0CTL1 |=
		  UCTR		// setting UCTR for transmitter mode
		| UCTXSTT; 	// setting UCTXSTT to generate START condition
	for (int i = 0; i < MESSAGE_SIZE; ++i) { // while have data to be transmitted
		UCB0TXBUF = CONFIGURE_CONTINUOUS_MEASUREMENT_MODE[i]; // load data to be trasmitted on UCBxTXBUF
		while ( !(IFG2 & UCB0TXIFG) ); // wait UCBxTXIFG are set, when set the buffer are empty
	}
	// UCB0CTL1 |= UCTXSTP; // setting UCTXSTP, STOP condition is generated
	// while (UCB0CTL1 & UCTXSTP); // wait STOP condition is ack


	P1OUT |= BIT0; // turn on led 1 if configuration ok


	// Master Receiver
	
	UCB0CTL1 |=
		  UCTR		// setting UCTR for transmitter mode
		| UCTXSTT; 	// setting UCTXSTT to generate START condition
	while ( !(IFG2 & UCB0TXIFG) ); // wait UCBxTXIFG are set, when set the buffer are empty
	UCB0TXBUF = 0x03; // load data to be trasmitted on UCBxTXBUF

	UCB0CTL1 &= ~UCTR;	// setting UCTR for receiver mode
	UCB0CTL1 |= UCTXSTT; 	// setting UCTXSTT to generate START condition
	while (UCB0CTL1 & UCTXSTT);

	// while have data to be received
	while (true) {
		P1OUT ^= BIT0; // toogle led 1

		short x, y, z; // compass coordinates
		int msb, lsb;

/* TODO  commented codes print registres A B C MSX LSX MSZ LSZ MSY LSY of magnetometer
 * put this codes on a function, will appear more clear
 */

/*

		while ( !(IFG2 & UCB0RXIFG) ); // wait UCBxRXIFG are set, when set the buffer are full
		int msb = UCB0RXBUF;
		putc('a');
		putc(' ');
		putc(' ');
		print(msb); // read data from UCBxRXBUF
		putc('\r'), putc('\n');

		while ( !(IFG2 & UCB0RXIFG) ); // wait UCBxRXIFG are set, when set the buffer are full
		int lsb = UCB0RXBUF;
		putc('b');
		putc(' ');
		putc(' ');
		print(lsb); // read data from UCBxRXBUF
		putc('\r'), putc('\n');
		
		while ( !(IFG2 & UCB0RXIFG) ); // wait UCBxRXIFG are set, when set the buffer are full
		putc('c');
		putc(' ');
		putc(' ');
		print(UCB0RXBUF);
		putc('\r'), putc('\n');

*/
		while ( !(IFG2 & UCB0RXIFG) ); // wait UCBxRXIFG are set, when set the buffer are full
		msb = UCB0RXBUF;
//		putc('m');
//		putc('x');
//		putc(' ');
//		print(msb);
//		putc('\r'), putc('\n');
		
		while ( !(IFG2 & UCB0RXIFG) ); // wait UCBxRXIFG are set, when set the buffer are full
		lsb = UCB0RXBUF;
//		putc('l');
//		putc('x');
//		putc(' ');
//		print(lsb);
//		putc('\r'), putc('\n');

		x = (msb << 8) + lsb;

		while ( !(IFG2 & UCB0RXIFG) ); // wait UCBxRXIFG are set, when set the buffer are full
		msb = UCB0RXBUF;
//		putc('m');
//		putc('z');
//		putc(' ');
//		print(msb);
//		putc('\r'), putc('\n');
		
		while ( !(IFG2 & UCB0RXIFG) ); // wait UCBxRXIFG are set, when set the buffer are full
		lsb = UCB0RXBUF;
//		putc('l');
//		putc('z');
//		putc(' ');
//		print(lsb);
//		putc('\r'), putc('\n');

		z = (msb << 8) + lsb;

		while ( !(IFG2 & UCB0RXIFG) ); // wait UCBxRXIFG are set, when set the buffer are full
		msb = UCB0RXBUF;
//		putc('m');
//		putc('y');
//		putc(' ');
//		print(msb);
//		putc('\r'), putc('\n');
		
		while ( !(IFG2 & UCB0RXIFG) ); // wait UCBxRXIFG are set, when set the buffer are full
		lsb = UCB0RXBUF;
//		putc('l');
//		putc('y');
//		putc(' ');
//		print(lsb);
//		putc('\r'), putc('\n');

		y = (msb << 8) + lsb;
		
		print("------------------------------\r\n");
		print("read of x: ");
		print(x);
		print("\r\n");
		print("read of y: ");
		print(y);
		print("\r\n");
		print("read of z: ");
		print(z);
		print("\r\n");
		print("------------------------------\r\n");

		putc('\r'), putc('\n');

	}

	// Ending I2C
	
	// if want stop transmission
	// XXX this line never will be executed in this context, because we have a infinit loop above
	UCB0CTL1 |= UCTXSTP; // setting UCTXSTP, STOP condition is generated

	// if want change slave address or transmitter/reciver mode
	// setting UCTXSTT communicate RESTART

	// if not-acknowledge was recived UCNACKIFG is set
}


void putc(char ch) {
	while(!(IFG2 & UCA0TXIFG));
	UCA0TXBUF = ch;
}

void print (const char msg[]) {
	for (int i = 0; msg[i] != '\0'; ++i)
		putc(msg[i]);	
}

void print (short value) {
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
