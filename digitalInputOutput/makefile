all: main.hex
	mspdebug --force-reset rf2500 "prog main.hex"

main.o: main.cpp
	msp430-g++ -c -Os -Wall -ffunction-sections -fdata-sections -mmcu=msp430g2553 -DF_CPU=16000000L main.cpp -o main.o -g

main.elf: main.o
	msp430-gcc -Os -Wl -gc-sections -u main -mmcu=msp430g2553 -o main.elf main.o -lm -g

main.hex: main.elf
	msp430-objcopy  -O ihex -R .eeprom main.elf main.hex

clean:
	rm main.o main.elf main.hex
