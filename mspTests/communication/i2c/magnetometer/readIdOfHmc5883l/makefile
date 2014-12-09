all: main.hex
	mspdebug --force-reset rf2500 "prog main.hex"

main.o: main.cpp
	msp430-g++ -c -Os -Wall -ffunction-sections -fdata-sections -mmcu=msp430g2553 -DF_CPU=16000000L main.cpp -o main.o -I/usr/include -g -fpermissive

main.elf: main.o
	msp430-gcc -Os -Wl -gc-sections -u main -mmcu=msp430g2553 -o main.elf main.o -lm -g

main.hex: main.elf
	msp430-objcopy  -O ihex -R .eeprom main.elf main.hex

clean:
	-@rm main.o main.elf main.hex 2>/dev/null || true

debug: main.hex
	mspdebug rf2500 gdb > .gdb.log & #open comunication do gdb listening by default in port 2000
	sleep 3 # wait open communication
	#open gdb and connect to localhost:2000
	msp430-gdb -tui --eval-command="target remote localhost:2000"  --eval-command="file main.elf" --eval-command="load main.elf"
	$(MAKE) erase
	
erase:
	mspdebug --force-reset rf2500 erase #limpa a memoria do controlador

cclean: erase clean
