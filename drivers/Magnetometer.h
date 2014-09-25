#pragma once

#include <avr/io.h>
#include <math.h>
#include <stdint.h>

namespace carInMaze {

	class Magnetometer {
		private:
			//TODO implementar todo tratamento de erros do i2c
			static void i2cConfigureBaundRate(long baund) {
				TWSR = 0;
				TWBR = ((F_CPU/baund)-16)/2;
			}

			static void i2cStart() {
				TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
				loop_until_bit_is_set(TWCR, TWINT);
			}

			static void i2cStop() {
				TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
				loop_until_bit_is_clear(TWCR, TWSTO);
			}

			static void i2cSend(const uint8_t value) {
				TWDR = value;
				TWCR = (1<<TWINT) | (1<<TWEN);
				loop_until_bit_is_set(TWCR, TWINT);
			} 

			static uint8_t i2cRead(bool last = false) {
				TWCR = (1<<TWINT) | (1<<TWEN) | ((last?0:1)<<TWEA); 
				loop_until_bit_is_set(TWCR, TWINT);
				return TWDR;
			}

			static void setRegisterPointer(uint8_t registerAdress) {
				i2cStart();
				i2cSend(0x3c);
				i2cSend(registerAdress);
				i2cStop();
			}

			static void setRegister(uint8_t registerAdress, uint8_t value) {
				i2cStart();
				i2cSend(0x3c);
				i2cSend(registerAdress);
				i2cSend(value);
				i2cStop();
			}

			void readCoordinates() {
				i2cStart();
				i2cSend(0x3d);
				NortX = (i2cRead() << 8) | i2cRead();
				NortZ = (i2cRead() << 8) | i2cRead();
				NortY = (i2cRead() << 8) | i2cRead(true);
				i2cStop();
			}


			int16_t NortX, NortY, NortZ;

			static double convertToDegree( const int16_t &a, const int16_t &b) { //TODO corrigir erros de calibracao
				if (a == 0) return ( b < 0 ? 90. : 270. );
				const double PI = 3.14159265;
				double heading = atan(static_cast<double>(b)/ static_cast<double>(a)) * 180./PI; 
				if (a < 0) return 180. - heading;
				if (b < 0) return -heading;
				else return 360. - heading;
			}

			int16_t NortXScaleFactor, NortYScaleFactor, NortXOffset, NortYOffset;

			void getCorrectionFactors(int16_t xMax, int16_t xMin, int16_t yMax, int16_t yMin) {
				int16_t xRange = xMax - xMin,yRange = yMax - yMin;
				NortXScaleFactor = yRange / xRange;
				if (NortXScaleFactor < 1) NortXScaleFactor = 1; 
				NortYScaleFactor = xRange / yRange;
				if (NortYScaleFactor < 1) NortYScaleFactor = 1; 
				NortXOffset = (xRange/2 - xMax) * NortXScaleFactor;
				NortYOffset = (yRange/2 - yMax) * NortYScaleFactor;
			}

			void applyCorrection() {
				NortX = NortXScaleFactor * NortX + NortXOffset;
				NortY = NortYScaleFactor * NortY + NortYOffset;
			}

			enum REGISTERS { REG_A = 0x00, REG_B = 0x01, REG_C = 0x02 };

		public:
			Magnetometer() { //permitir a configuracao via construtor
				i2cConfigureBaundRate(100000L);		
				setRegister(REG_A, 0x70);
				setRegister(REG_B, 0xb0);
				setRegister(REG_C, 0x00);
			}
			
			int16_t xMax, xMin, yMax, yMin;

			void startCalibrate() {
				xMax = INT16_MIN;
				xMin = INT16_MAX;
				yMax = INT16_MIN;
				yMin = INT16_MAX;
			}

			// Calibra o magnetometro
			void calibrateStep() {
				measure();
				if (NortX > xMax) xMax = NortX;
				if (NortX < xMin) xMin = NortX;
				if (NortY > yMax) yMax = NortY;
				if (NortY < yMin) yMin = NortY;
			}

			void calibrateFinish() {
				getCorrectionFactors(xMax, xMin, yMax, yMin);
				applyCorrection();
			}


			void measure() { //TODO criar um enum de configuracoes
				setRegisterPointer(0x03);
				readCoordinates();
			}

			const int16_t &getNortX() { return NortX; }
			const int16_t &getNortY() { return NortY; }

			double getHorizontalDegree() const {
				return convertToDegree(NortX, NortY);
			}

			double getVerticalDegree() const { //ignored in land veicles
				return convertToDegree(NortX, NortZ);
			}
	};

}	
