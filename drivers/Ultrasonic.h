#pragma once

#include <avr/io.h>
#include <util/delay.h>
#include "PinPort.h"

namespace carInMaze {
	
	template <
		const uint16_t SAMPLES = 10,
		const uint8_t CLOCKS_PER_MICROSECOND = F_CPU/1000000L,
		const uint8_t MIN_DISTANCE = 3,
		const uint8_t CENTIMETER_RATIO = 58,
		const uint8_t CONSTANT_CORRECTION = 0
		>
	class Ultrasonic {
			static const uint16_t MEASURE_INTERVAL = 100;
			
			uint8_t clocksWidth[SAMPLES];

			const PinPort TRIGGER, ECHO;
		private:
			void trigger() const {
				TRIGGER.PORT &= ~_BV(TRIGGER.PIN);
				_delay_us(2);
				TRIGGER.PORT ^= _BV(TRIGGER.PIN); //rising edge
				_delay_us(10);
				TRIGGER.PORT ^= _BV(TRIGGER.PIN); //falling edge
			}

		public:
			Ultrasonic(const PinPort trigger, const PinPort echo) : TRIGGER(trigger), ECHO(echo) {
				//TODO tornar a configuracao de portas generica, primeiro insigth use enums
				DDRB |= _BV(trigger.PIN); //WARNING funcionamento normal apenas com trigger na porta B
			}

			void measure() {
				for (auto i = 0; i < SAMPLES; ++i) {
					clocksWidth[i] = getSingleMeasure();
					_delay_ms(MEASURE_INTERVAL);
				}
			}

			uint8_t getGreatestMeasure() const {
				auto sample = 0;
				for (auto i = 0; i < SAMPLES; ++i)
					if ( clocksWidth[i] > sample )
						sample = clocksWidth[i];
				return sample;
			}

			uint8_t getMeanMeasure() const {
				uint32_t sum = 0;
				for (auto i = 0; i < SAMPLES; ++i)
					sum += clocksWidth[i];
				return sum/SAMPLES;
			}

			uint8_t getLowestMeasure() const {
				auto sample = UINT8_MAX;
				for (auto i = 0; i < SAMPLES; ++i)
					if ( clocksWidth[i] < sample )
						sample = clocksWidth[i];
				return sample;
			}

			uint8_t getSingleMeasure() { //XXX atualmente o funcionamento correto eh apenas para a porta b TODO substituir PINB port uma relacao de ECHO.PORT que gere o PINB
				TCCR4B = 1; //habilita o timer 4, configurado com o clock de io
				trigger();
				loop_until_bit_is_set(/*ECHO.PORT*/PINB, ECHO.PIN); //TODO definir timeout
				TCNT4 = 0; //zera o timer
				loop_until_bit_is_clear(/*ECHO.PORT*/PINB, ECHO.PIN); //TODO definir timeout
				TCCR4B = 0; //desabilita o timer 4, configura sem fonte de clock
				uint16_t clocks = TCNT4;
				return clocks / CLOCKS_PER_MICROSECOND / CENTIMETER_RATIO + CONSTANT_CORRECTION; //pega o valor no timer
			}
				
				
	};
				
}
