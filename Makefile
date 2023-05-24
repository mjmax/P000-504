# Main application file name
MAIN_APP = P000-504

# Output directory
OUTPUT_DIR = output
OBJ_DIR = obj

# Main hex file path in Windows format
MAIN_HEX_PATH = $(OUTPUT_DIR)/$(MAIN_APP).hex

# Compiler and other settings
AVR_DIR = W:\utility\Arduino\hardware\tools\avr
CC = $(AVR_DIR)\bin\avr-gcc.exe
OBJCOPY = $(AVR_DIR)\bin\avr-objcopy.exe
AVRDUDE = $(AVR_DIR)\bin\avrdude.exe -C "$(AVR_DIR)\etc\avrdude.conf"

# Options for avr-gcc
CFLAGS = -g -Os -mmcu=atmega2560

# Options for HEX file generation
HFLAGS = -j .text -j .data -O ihex

# Options for avrdude to burn the hex file
#DUDEFLAGS += -v
#DUDEFLAGS = -c stk500v2
#DUDEFLAGS = -c avrispMkII
DUDEFLAGS = -c wiring
#DUDEFLAGS = -c arduino
DUDEFLAGS += -p m2560
DUDEFLAGS += -P COM10
DUDEFLAGS += -b 115200
DUDEFLAGS += -D
DUDEFLAGS += -F
DUDEFLAGS += -U flash:w:$(MAIN_HEX_PATH):i

# Directory for source and header files
SRC_DIR = scr

# Sources files needed for building the application
SRC = $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

# The headers files needed for building the application
INCLUDE = -I$(SRC_DIR)

# Build target
all: build burn

# Rebuild target
rebuild: clean build

build: $(MAIN_HEX_PATH)

$(MAIN_HEX_PATH): $(OUTPUT_DIR)/$(MAIN_APP).elf
	$(OBJCOPY) $(HFLAGS) $< $@

$(OUTPUT_DIR)/$(MAIN_APP).elf: $(OBJ) | $(OUTPUT_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

# Command to burn the hex file
burn:
	$(AVRDUDE) $(DUDEFLAGS)

$(OUTPUT_DIR):
	mkdir $(OUTPUT_DIR)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

clean:
	rmdir /s /q $(OUTPUT_DIR) $(OBJ_DIR)

.PHONY: all build burn clean
