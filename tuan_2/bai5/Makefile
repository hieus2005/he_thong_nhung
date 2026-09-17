CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -mcpu=cortex-m3 -mthumb -O2 -Wall
CFLAGS += -DUSE_STDPERIPH_DRIVER -DSTM32F10X_MD

CFLAGS += -I./inc -I./lib/SPL/inc -I./lib/CMSIS

# Khai báo file ngu?n: main.c và uart.c c?a b?n
SRCS = ./src/main.c ./src/uart.c
SRCS += ./lib/SPL/src/stm32f10x_usart.c ./lib/SPL/src/stm32f10x_gpio.c 
SRCS += ./lib/SPL/src/stm32f10x_rcc.c ./lib/SPL/src/misc.c ./lib/SPL/src/stm32f10x_tim.c
SRCS += ./src/startup_stm32f10x_md.s

LDFLAGS = -T stm32_flash.ld -Wl,--gc-sections

all: main.bin

main.elf: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) $(LDFLAGS) -o $@

main.bin: main.elf
	$(OBJCOPY) -O binary $< $@

flash: main.bin
	st-flash write main.bin 0x8000000

clean:
	rm -f *.elf *.bin
