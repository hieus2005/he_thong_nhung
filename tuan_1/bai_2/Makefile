CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

CFLAGS=-mcpu=cortex-m3 -mthumb -Os -nostdlib

all: blink.bin

blink.elf: startup_stm32f103.s main.c
	$(CC) $(CFLAGS) startup_stm32f103.s main.c -T stm32f103.ld -o blink.elf

blink.bin: blink.elf
	$(OBJCOPY) -O binary blink.elf blink.bin

flash: blink.bin
	st-flash write blink.bin 0x08000000

clean:
	rm -f *.elf *.bin
