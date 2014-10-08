#include <msp/msp430g2553.h>

main() {
	P1SEL &= ~BIT0;
	P1DIR |= BIT0;	
	
	TACTL |= TACLR;
	TACTL = TASSEL_0 + MC_2 + ID_3;

	const unsigned long TIME = 0xff;

	while (true) {

		P1OUT ^= BIT0;

		TACTL |= 2 << MC0;
		while (TAR < TIME);

		P1OUT ^= BIT0;
		
		TACTL |= 2 << MC0;
		while (TAR < TIME);
	}
}

