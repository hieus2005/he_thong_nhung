CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -mcpu=cortex-m3 -mthumb -O2 -Wall
CFLAGS += -DSTM32F10X_MD
CFLAGS += -I./inc -I./lib/SPL/inc -I./lib/CMSIS

SRCS = ./src/main.c ./src/uart.c ./src/dma.c ./src/i2c.c ./src/mpu6050.c
SRCS += ./src/startup_stm32f10x_md.s

OBJS = $(SRCS:.c=.o)
OBJS := $(OBJS:.s=.o)

LDFLAGS = --specs=nano.specs --specs=nosys.specs -T stm32_flash.ld -Wl,--gc-sections

all: main.bin

main.elf: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

main.bin: main.elf
	$(OBJCOPY) -O binary $< $@

flash: main.bin
	st-flash write main.bin 0x8000000

clean:
	rm -f src/*.o lib/*/*.o *.elf *.bin
