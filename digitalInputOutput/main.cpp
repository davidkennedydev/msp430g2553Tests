#include <msp430g2553.h>

/** Description:
 * change leds on while you press button
 * display green light until press button,
 * then display red light while press button
 */

main() {
	P1SEL |= 0x00; //no special function of pins on port P1
	P1DIR |= BIT0 | BIT6; // configure output mode pins 0 and 6, corresponding to leds 1 (red) and 2 (green)
	P1REN |= BIT3; // enable pin 3 do use pullup or pulldown resistors
	P1OUT |= BIT3; // configure pin 3 (switch 2 SW2) who pullup, because when pressed the switch pull GND on wire

	while (true)
		if (P1IN & BIT3) { // if button isn't pressed (pin not grounded, so have pin value 1)
			P1OUT |= BIT6; // set pin 6 on
			P1OUT &= ~BIT0; // set pin 0 off
		} else {
			//exemplo de inline assembly
			asm(
				"bic %[pin6], %0 ; bis %[pin0], %0 "	// set pin 6 off // set pin 0 on

				: // parametros de escrita (rvalue)
					"=r" (P1OUT) // parametro %0 , poderia ser entitulado de forma mais legivel
				: // parametros de leitura (lvalue)
					[pin0] "i" (BIT0), //parametro %1 ou %[pin0] ("i" diz que o parametro BIT0 é uma constante)
					[pin6] "i" (BIT6) //parametro %2 ou %[pin6]
			);
		}
}
