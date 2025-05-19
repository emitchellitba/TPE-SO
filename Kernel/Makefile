include Makefile.inc

KERNEL=kernel.bin


SOURCES=$(wildcard *.c)
SOURCES_ASM=$(wildcard asm/*.asm)
SOURCES_DRIVERS=$(wildcard drivers/*.c)
SOURCES_INTERRUPTS=$(wildcard interrupts/*.c)
SOURCES_LIB=$(wildcard lib/*.c)
SOURCES_MEMORY=$(wildcard memory/*.c)


OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
OBJECTS_DRIVERS=$(SOURCES_DRIVERS:.c=.o)
OBJECTS_INTERRUPTS=$(SOURCES_INTERRUPTS:.c=.o)
OBJECTS_LIB=$(SOURCES_LIB:.c=.o)
OBJECTS_MEMORY=$(SOURCES_MEMORY:.c=.o)


LOADERSRC=loader.asm
LOADEROBJECT=$(LOADERSRC:.asm=.o)


STATICLIBS=


INCLUDE_DIRS=$(shell find ./include -type d)
INCLUDE_FLAGS=$(foreach dir,$(INCLUDE_DIRS),-I$(dir))
ASM_INCLUDE_FLAGS=-Iasm


all: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM) $(OBJECTS_DRIVERS) $(OBJECTS_INTERRUPTS) $(OBJECTS_MEMORY) $(OBJECTS_LIB)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_DRIVERS) $(OBJECTS_INTERRUPTS) $(OBJECTS_MEMORY) $(STATICLIBS) $(OBJECTS_LIB)
	$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o kernel.elf $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_DRIVERS) $(OBJECTS_INTERRUPTS) $(OBJECTS_MEMORY) $(STATICLIBS) $(OBJECTS_LIB)


%.o: %.c
	$(GCC) $(GCCFLAGS) $(CFLAGS) $(INCLUDE_FLAGS) -c $< -o $@


%.o : %.asm
	$(ASM) $(ASMFLAGS) $(ASM_INCLUDE_FLAGS) $< -o $@


$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)


clean:
	rm -rf asm/*.o drivers/*.o interrupts/*.o lib/*.o memory/*.o *.o *.bin kernel.bin

.PHONY: all clean

