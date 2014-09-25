#include "../../Magnetometer.h"
#include "/home/david/avrTests/USART/Serial.h"
#include <avr/io.h>
#include <util/delay.h>

int main() {

	using carInMaze::Magnetometer;
	
	Serial usb;
	Magnetometer magnetometer;

	repeatedScope: //as linhas abaixo vão repetir continuamente
	magnetometer.measure();
	usb << magnetometer.getHorizontalDegree() << '\n';
	_delay_ms(500);

	goto repeatedScope;
}
