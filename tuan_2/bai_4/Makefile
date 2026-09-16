CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
CFLAGS  = -mcpu=cortex-m3 -mthumb -nostdlib

# File mục tiêu
TARGET  = firmware

all: flash

$(TARGET).elf: startup.s main.c
	$(CC) $(CFLAGS) -T linker.ld $^ -o $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

flash: $(TARGET).bin
	st-flash write $< 0x08000000

clean:
	rm -f *.elf *.bin *.o