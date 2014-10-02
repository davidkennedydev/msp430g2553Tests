#include <msp/msp430g2553.h>

main() {
	WDTCTL = WDTPW + WDTHOLD;

	P1SEL |= 0x00;
	P1DIR |= BIT0 | BIT6; // configure the pin 0 of port 1 to output, other's are in input mode
	P1REN |= BIT3; // configure switch 2 who pullup

#define ASM_CMD(INS, SRC, DEST) asm( #INS" %1, %0;": "=r" (DEST): "i" (SRC) );
#define BIS(SRC, DEST) ASM_CMD(bis.b, SRC, DEST)
#define BIC(SRC, DEST) ASM_CMD(bic.b, SRC, DEST)

#define BS asm( "bis.b %1, %0;": "=r" (
#define _BS ): "i" (
#define BS_ ) );

	BS P1OUT _BS BIT0 BS_
	BIS(BIT6, P1OUT)

	
/*
	while (true) {
		if (P1IN & BIT3) {
			//P1OUT |= BIT6;
			P1OUT &= ~BIT0;
		} else {
			P1OUT |= BIT0;
			P1OUT &= ~BIT6;
		}
	}
*/
		
}
