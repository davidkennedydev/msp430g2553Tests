#pragma once 

#include <avr/io.h>
#include <avr/delay.h>

class Motor {
		static const uint8_t STOP = 0xff, RUNNING = 0;;

		volatile uint16_t *positivePole, *negativePole;

	private:	

		uint8_t getDuty(uint8_t strength) const {
			return STOP + (static_cast<int>(RUNNING) - static_cast<int>(STOP))/100. * strength; //partindo de parado, n partes da distancia de parado a andando, tal que n e um numero de partes das 100 de parado a andando
		}

	public:
		enum MOTOR_PINS { M1, M3 };

		Motor(MOTOR_PINS pins)
		{
			//TODO atualmente so funciona com os pinos especificos OC3A e OC3B
			//TODO fornecer as configuracoes de pwm disponiveis para o motor
#define PREPARE_PWM(N) \
TCCR ## N ## A = _BV(COM ## N ## A1) | _BV(COM ## N ## A0) | _BV(COM ## N ## B1) | _BV(COM ## N ## B0) | _BV(WGM ## N ## 0); \
TCCR ## N ## B |= _BV(CS00) | _BV(WGM ## N ## 2); 

			switch (pins) {
				case M1:
					DDRB = _BV(PB5) | _BV(PB6);
					PREPARE_PWM(1);
					positivePole = &OCR1B;
					negativePole = &OCR1A;
					break;
				case M3:
					DDRE = _BV(PE3) | _BV(PE4);
					PREPARE_PWM(3);
					positivePole = &OCR3A;
					negativePole = &OCR3B;
					break;
				
			}
		}

		void forward(uint8_t strength = 100) {
			*positivePole = getDuty(strength);
			*negativePole = 0xff;
		}

		void backward(uint8_t strength = 100) {
			*positivePole = 0xff;
			*negativePole = getDuty(strength);
		}
		void stop() {
			*positivePole = *negativePole = 0xff;
		}
};

		
	

