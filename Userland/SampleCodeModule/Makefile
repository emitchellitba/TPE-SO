include ../Makefile.inc

MODULE=0000-sampleCodeModule

SOURCES=$(wildcard [^_]*.c) $(wildcard [^_]*/[^_]*.c)
SOURCES_ASM=$(wildcard */*.asm) $(wildcard *.asm)

OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)

INCLUDE_DIRS=$(shell find include -type d)
INCLUDE_FLAGS=$(addprefix -I,$(INCLUDE_DIRS))

all: $(MODULE)

$(MODULE): $(OBJECTS) $(OBJECTS_ASM) _loader.o
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld _loader.o -o ../$(MODULE).bin $(OBJECTS) $(OBJECTS_ASM)
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld --oformat=elf64-x86-64 _loader.o -o ../$(MODULE).elf $(OBJECTS) $(OBJECTS_ASM)

%.o: %.c
	$(GCC) $(GCCFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf *.o */*.o 

.PHONY: all clean print
