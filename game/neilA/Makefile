# Compiler and linker settings
COMPILER_DIR = /opt/libs/avr8-gnu-toolchain/bin
CC = $(COMPILER_DIR)/avr-gcc
OBJCOPY = $(COMPILER_DIR)/avr-objcopy
AVRSIZE = $(COMPILER_DIR)/avr-size

# Microcontroller and project directories
MCU = atmega32
PROJECT_NAME = neilA
PROJECT_DIR = /home/vladimr/Desktop/git/sw-c/game/neilA
BUILD_DIR = $(PROJECT_DIR)/build_main
SRC_DIR = $(PROJECT_DIR)
SRC = $(SRC_DIR)/main.c

# File extensions
ELF = $(BUILD_DIR)/$(PROJECT_NAME).elf
HEX = $(BUILD_DIR)/$(PROJECT_NAME).hex

# Default build option
BUILD_TYPE ?= mini
INFO_TARGET ?= $(BUILD_DIR)/$(PROJECT_NAME).elf

# Compiler flags for different build types
CFLAGS_DEBUG = -Wall -g -mmcu=$(MCU)
CFLAGS_MINI = -Os -mmcu=$(MCU) -ffunction-sections -fdata-sections -Wl,--gc-sections
CFLAGS_MICRO = -Os -mmcu=$(MCU) -ffunction-sections -fdata-sections -Wl,--gc-sections \
               -fomit-frame-pointer -fno-unroll-loops -fno-rtti -fno-exceptions -fstrength-reduce

# Select the appropriate CFLAGS based on the build type
ifeq ($(BUILD_TYPE),debug)
  CFLAGS = $(CFLAGS_DEBUG)
endif
ifeq ($(BUILD_TYPE),mini)
  CFLAGS = $(CFLAGS_MINI)
endif
ifeq ($(BUILD_TYPE),micro)
  CFLAGS = $(CFLAGS_MICRO)
endif

# Target for building the ELF file
$(ELF): $(SRC)
	$(CC) $(CFLAGS) -o $(ELF) $(SRC)

# Target for converting the ELF to HEX file
$(HEX): $(ELF)
	$(OBJCOPY) -j .text -j .data -R .comment -R .debug --strip-all -O ihex $(ELF) $(HEX)

# Default target (builds debug by default)
all: $(HEX)

info:
	$(AVRSIZE) $(INFO_TARGET)
# Hint
#  text (program code)
#    minimize code, use `static inline` functions
#  data (initialized) ; bss (uninitialized)
#    global&static variables
#  dec and hex (total memory used)

# Clean the build directory
clean:
	rm -f $(ELF) $(HEX)

# Print the build type
print-build:
	@echo "Building $(PROJECT_NAME) with $(BUILD_TYPE) settings..."


# Usage:	make all info BUILD_TYPE=mini
#		make info INFO_TARGET=<program>.elf
