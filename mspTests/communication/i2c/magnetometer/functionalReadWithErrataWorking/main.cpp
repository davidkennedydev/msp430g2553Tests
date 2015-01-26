/* 
 * Print all printable characters on usb channel using UART mode of USCI module
 * the print is based in busy wait to transfer characters
 */

#include "msp/msp430g2553.h"

void configureDCO();
  
void configureUART();
void putc(char);
void print (const char[]);
void print (short);
void printStatus();

void i2cConfigure();
void i2cTransaction();
void i2cTransmit(char);
char i2cReceive();
void i2cStop();
	 
main() {
	configureDCO();
	configureUART();

	i2cConfigure();
	print("[system] I2C configured\r\n");

	i2cTransaction();
	i2cTransmit(0x00);
	i2cTransmit(0x78);
	i2cTransmit(0x01);
	i2cTransmit(0xa0);
	i2cTransmit(0x02);
	i2cTransmit(0x00);
	i2cStop();
	//print("test-1\r\n");

	// infinit loop, show continually the measuread values
	while (true) {
		__delay_cycles(1000000*0.1);

		//print("test0\r\n");
		i2cTransaction();

		// allocate space to the measured values, (x,y,z) coordinates, high byte (h prefix), low byte (l prefix)
		unsigned char hx, lx, hz, lz, hy, ly;

		hx = i2cReceive(); // load data from buffer to local memory
		lx = i2cReceive(); // load data from buffer to local memory
		hz = i2cReceive(); // load data from buffer to local memory
		lz = i2cReceive(); // load data from buffer to local memory
		hy = i2cReceive(); // load data from buffer to local memory
		ly = i2cReceive(); // load data from buffer to local memory

		i2cStop();
		i2cTransmit(0x03);
		i2cStop();
		//print("test1\r\n");

		// compose (x, y, z) values
		short x, y, z;
		x = (static_cast<short>(hx) << 8) | lx; // compose x MSB and LSB
		z = (static_cast<short>(hz) << 8) | lz; // compose z MSB and LSB
		y = (static_cast<short>(hy) << 8) | ly; // compose y MSB and LSB

		//print("test2\r\n");
		//print (x, y, z) measured values
		print("( ");
		print(x);
		print(", ");
		print(y);
		print(", ");
		print(z);
		print(" )");
		print("\r\n");
		//print("test3\r\n");
	}
}


void configureDCO() {
	WDTCTL = WDTPW + WDTHOLD; // Stop WDT

	// Set DCO to 1MHz
	BCSCTL1 = CALBC1_1MHZ; 
	DCOCTL = CALDCO_1MHZ; 

//	__delay_cycles(1000000);
}

void configureUART() {
	UCA0CTL1 |= UCSWRST; // put USCI in reset mode

	// P1.1 = RXD, P1.2=TXD
	P1SEL  |= BIT1 | BIT2;
	P1SEL2 |= BIT1 | BIT2;

	UCA0CTL1 |= UCSSEL_3; // Use SMCLK

	// Set baud rate to 9600 with 1MHz clock (Data Sheet 15.3.13) 1MHz / 104
	UCA0BR0 = 104;
	UCA0BR1 = 0;

	UCA0MCTL = UCBRS0; // Modulation UCBRSx = 1, correction for the fractional part ( the rest, 9600 - 1MHz / 104)
	UCA0CTL1 &= ~UCSWRST; // put USCI in operad mode (removing reset mode)

	/*End configuration hardware UART */
	print("[system] uart configured\r\n");
};
	

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

void printStatus() {
	print("Status:\n\r");
	#define __PRINT_STATUS(X, MSG) if (UCB0STAT & X) print(#MSG "! ");
	__PRINT_STATUS(UCNACKIFG, nack);
	__PRINT_STATUS(UCALIFG, arbitration lost);
	__PRINT_STATUS(UCGC, receive general call);
	__PRINT_STATUS(UCBBUSY, bus busy);
	__PRINT_STATUS(UCSCLLOW, SCL low);
	__PRINT_STATUS(UCSTPIFG, stop received);
	__PRINT_STATUS(UCSTTIFG, start received);
	print("done\n\r");
}

void i2cConfigure() {
	//configure i2 pins
	P1SEL |= BIT6 | BIT7;
	P1SEL2|= BIT6 | BIT7;

	UCB0CTL1 |= UCSWRST; // disable

	UCB0CTL0 =  		// USCI_B0 control register
		UCMST		// set master mode	
		| UCMODE_3 	// I2C mode select
		| UCSYNC; 	// synchronous


	UCB0CTL1 = 		// UCSI_B0 control register
		UCSSEL_2	//select clock source, SMCLK
		| UCSWRST;

	UCB0I2CSA = 0x1e;	// write desired slave address to the UCBxI2CSA, magnetometer address
	UCB0BR0 = 4;
	UCB0BR1 = 0;
	UCB0CTL1 &= ~UCSWRST; // enable
	UCB0I2CIE |= UCNACKIE;
	__enable_interrupt();
}

void i2cTransmit(char data) {
	if (!(UCB0CTL1 & UCTR)) { //if not in transmitting mode
		while (UCB0CTL1 & UCTXSTP); // wait stop

		UCB0CTL1 |=
			  UCTR		// setting UCTR for transmitter mode
			| UCTXSTT; 	// setting UCTXSTT to generate START condition
		__delay_cycles(6000);
	}
	while (!(IFG2 & UCB0TXIFG));
	UCB0TXBUF = data;
}

char i2cReceive() {
	if (UCB0CTL1 & UCTR) { //if not in receiver mode
		while (UCB0CTL1 & UCTXSTP); // wait stop

		UCB0CTL1 &= ~UCTR; 	// setting UCTXSTT to generate START condition
		UCB0CTL1 |= UCTXSTT; 	// setting UCTXSTT to generate START condition
		while (UCB0CTL1 & UCTXSTT); // wait start be send
		__delay_cycles(6000);
	}
	while (!(IFG2 & UCB0RXIFG));
	char data = UCB0RXBUF;

	return data;
}

void i2cStop() {
	UCB0CTL1 |= UCTXSTP; //send stop
}

void i2cTransaction() {
	UCB0CTL1 |= UCSWRST; // disable, stop i2c communication, high impendance SDA and SCL, clear i2c status register
	UCB0CTL1 &= ~UCSWRST; // put USCI in operad mode (removing reset mode)
}


#pragma vector=USCIAB0RX_VECTOR
__interrupt void uartI2cReceiveInterrupt() {
	if (UCB0STAT & UCNACKIFG)
		IFG2 &= ~UCB0TXIFG;
}

#define TRAPINT_VECTOR (0 * 2u)
#pragma vector=TRAPINT_VECTOR
__interrupt void trapIntISR() {
	__no_operation();
}


