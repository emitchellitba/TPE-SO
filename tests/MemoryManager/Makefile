include ./include/Makefile.inc

SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:.c=.o)
TARGET  := MemoryManagerTest

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LINKER) $(LINKER_FLAGS) $(OBJECTS) -o ../$(TARGET).out
	@rm -rf $(OBJECTS) # Clean .o files after generating the .out

%.o : %.c
	$(COMPILER) $< $(COMPILER_FLAGS) -o $@

clean:
	@rm -rf $(OBJECTS)
	@rm -rf ../$(TARGET).out

.PHONY: all clean $(TARGET)
