CC = ../gcc-arm-none-eabi-9-2020-q2-update/bin/arm-none-eabi-gcc
CFLAGS = -mcpu=cortex-a7 -fpic -ffreestanding
CSRCFLAGS = -O2 -Wall -Wextra
LFLAGS = -ffreestanding -O2 -nostdlib
DIR = .
OBJ:= os.o boot.o mm.o atag.o

IMG_NAME = raspOS.img

build: $(OBJ) $(DIR)/*.h
	$(CC) -T linker.ld -o $(IMG_NAME) $(LFLAGS) *.o
$(DIR)/%.o: $(DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(CSRCFLAGS)
$(DIR)/%.o: $(DIR)/%.S
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -rf *.o *.elf *.img
qemu: 
	qemu-system-arm -m 1024 -M raspi2 -serial stdio -kernel $(IMG_NAME)
