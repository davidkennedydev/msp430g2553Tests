CC=avr-g++
CXXFLAGS=-std=c++1y -fpermissive -c -g -Os -Wall -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega2560 -DF_CPU=16000000L -MMD -DUSB_VID=null -DUSB_PID=null -DARDUINO=105
SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)

all: clean $(OBJ)
	@echo "Hardware Done."

.cpp.o:
	@echo "compiling " $<
	@$(CC) $(CXXFLAGS) -o $@ -c $<

clean:
	@echo "cleaning"
	@-rm -rf $(OBJ) main.hex main.elf main.eep main.d
upload: main.hex
	avrdude -patmega2560 -cwiring -P/dev/ttyACM0 -b115200 -D -Uflash:w:main.hex:i

main.hex: main.o
	avr-gcc -Os -Wl,--gc-sections,--relax -mmcu=atmega2560 -o main.elf main.o -Lmain.tmp -lm 
	avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 main.elf main.eep
	avr-objcopy -O ihex -R .eeprom main.elf main.hex
