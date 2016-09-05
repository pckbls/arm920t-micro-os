CC = arm-none-eabi-gcc

CFLAGS  = -Wall -Wextra -ffreestanding -mcpu=arm920t -O2 -std=c99
CFLAGS += -Iinclude -I.

ASFLAGS = -Iinclude -I.

# Kernel-Objektdatein
KERNELOBJ  = driver/leds.o driver/serial.o driver/systemtimer.o driver/cp15_asm.o
KERNELOBJ += system/exceptions.o system/exceptions_asm.o \
             system/memory.o system/init_asm.o system/threads.o \
             system/cpu.o system/cpu_asm.o system/syscalls.o system/process.o
KERNELOBJ += util/ringbuffer.o util/memory.o
KERNELOBJ += util/functions.o

# Von Kernel und Userprogramm gemeinsam genutzte Dateien
USEROBJ += system/lib.o system/lib_asm.o

# Datein fÃ¼r das Userprogramm
USEROBJ += main.o

all: kernel.img

clean:
	rm -f kernel kernel.bin kernel.bin.gz kernel.img loader.o kernel.o user.o $(KERNELOBJ) $(USEROBJ)

kernel.o: $(KERNELOBJ)
	arm-none-eabi-ld -r $(KERNELOBJ) -o kernel.o

user.o: $(USEROBJ)
	arm-none-eabi-ld -r $(USEROBJ) -o user.o

kernel: kernel.o user.o loader.o
	arm-none-eabi-ld -T kernel.lds -o kernel

kernel.bin: kernel
	arm-none-eabi-objcopy -Obinary \
		--set-section-flags .bss=contents,alloc,load,data \
		kernel kernel.bin

kernel.bin.gz: kernel.bin
	gzip -f kernel.bin

kernel.img: kernel.bin.gz
	mkimage -A arm -T standalone -C gzip -a 0x20000000 -d kernel.bin.gz kernel.img

upload: kernel.img
	arm-install-image kernel.img
