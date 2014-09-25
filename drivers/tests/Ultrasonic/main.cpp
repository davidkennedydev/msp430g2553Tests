#include "../../Ultrasonic.h"
#include "/home/david/avrTests/USART/Serial.h"
#include <avr/io.h>
#include <util/delay.h>

int main() {

	using carInMaze::Ultrasonic;
	
	Serial usb;
	Ultrasonic<> ultrasonic({PB4, DDRB}, {PB5, DDRB});

	for (;;_delay_ms(500)) {
		ultrasonic.measure();
		usb << ultrasonic.getMeanMeasure() << '\n';
	}
}
