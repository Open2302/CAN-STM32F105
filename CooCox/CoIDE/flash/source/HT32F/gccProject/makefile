#-------------------------------------------------------------------------------
#
#    CooCox Cortex M3 Flash Programmer(CoFlash) -- Flash Algorithm makefile
# 
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#    User-modifiable options
#-------------------------------------------------------------------------------

# Set the Target output(LPC17xx_FlashSize)
# HT32F_127  (HT32F 127KB Flash)
# HT32F_31   (HT32F 31KB Flash)
# HT32F_16   (HT32F 16KB Flash)
# HT32F_8    (HT32F  8KB Flash)

FLASH_CHIP = HT32F_127

# Output file basename
OUTPUT := $(FLASH_CHIP)


# Optimization level, put in comment for debugging
OPTIMIZATION = -O0


#flash algorithm path
FLASH_ALGO_DIR := ../../..


# flash algorithm framework path
FRAMEWORK_DIR := ../../framework


# flash algorithm path
ALGO_DIR := ..


LDS := $(FRAMEWORK_DIR)/target.ld



# Output directories
BIN_DIR := bin
OBJ_DIR := obj

#-------------------------------------------------------------------------------
#    Tools
#-------------------------------------------------------------------------------

# Tool suffix when cross-compiling
CROSS_COMPILE := arm-none-eabi-

# Compilation tools
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld
OBJCOPY := $(CROSS_COMPILE)objcopy
OBJDUMP := $(CROSS_COMPILE)objdump
READELF := $(CROSS_COMPILE)readelf
RM    := rm -rf   
MKDIR := mkdir_cygwin
COPY  := cp 

# includes
INCLUDES  = -I$(FRAMEWORK_DIR)/

# defines
DEFINES = -D$(FLASH_CHIP)

TARGET_OPTS = -mcpu=cortex-m0 -mthumb
#position-independent options
PI_OPTS = -fPIC -fPIE -msingle-pic-base -mpic-register=9


CFLAGS = $(TARGET_OPTS)
#CFLAGS += -g -Wall -mlong-calls
CFLAGS += -g -Wall 
CFLAGS += $(OPTIMIZATION) $(INCLUDES) $(DEFINES) $(PI_OPTS)
LDFLAGS = $(OPTIMIZATION)  -nostartfiles $(TARGET_OPTS) -Wl,-Map=$(OUTPUT).map,--cref


#-------------------------------------------------------------------------------
#    Files
#-------------------------------------------------------------------------------

# Directories where source files can be found

VPATH  = $(FRAMEWORK_DIR)/
VPATH += $(ALGO_DIR)/


# Objects built from C source files

FRAMEWORK_SRCS := FlashExecFnc.c
      
ALGO_SRCS := $(wildcard $(ALGO_DIR)/*.c)
ALGO_SRCS := $(subst $(ALGO_DIR)/,, $(ALGO_SRCS)) 


SRCS := $(FRAMEWORK_SRCS) $(ALGO_SRCS)

C_OBJECTS := $(patsubst %.c,%.o,$(SRCS)) 

# Append OBJ and BIN directories to output filename
OUTPUT := $(BIN_DIR)/$(OUTPUT)

#-------------------------------------------------------------------------------
#    Rules
#-------------------------------------------------------------------------------

TARGET_ELF :=  $(OUTPUT).elf
C_OBJECTS_EXT := $(addprefix $(OBJ_DIR)/, $(C_OBJECTS))
C_DEPS_EXT := $(patsubst %.o, %.d, $(C_OBJECTS_EXT))


all:  $(OBJ_DIR) $(BIN_DIR)  $(TARGET_ELF)
$(BIN_DIR):
	@echo
	$(MKDIR) -p $@
$(OBJ_DIR):
	$(MKDIR) -p $@


$(TARGET_ELF) : $(C_OBJECTS_EXT)
	$(CC) $(LDFLAGS) -T$(LDS) -o $@ $^
	$(OBJDUMP) -D $@ >  $(@:.elf=.dis)
	$(OBJDUMP) -S $@ >  $(@:.elf=.src)
	$(READELF) -a $@ >  $(@:.elf=.info)
	$(OBJCOPY) -O binary $@ $(@:.elf=.bin)
	$(COPY) $@ $(FLASH_ALGO_DIR)

$(C_OBJECTS_EXT) : $(SRCS) 
$(OBJ_DIR)/%.d : %.c 
	$(CC) -MM -MT $(@:.d=.o) $(CFLAGS) $< > $@
	
$(OBJ_DIR)/%.o :%.c
	$(CC) $(CFLAGS) -c $< -o $@

-include $(C_DEPS_EXT)


.PHONY : clean
clean:
	-$(RM) $(OBJ_DIR)/*.o $(OBJ_DIR)/*.d $(BIN_DIR)/*.bin $(BIN_DIR)/*.elf  $(BIN_DIR)/*.map $(BIN_DIR)/*.dis $(BIN_DIR)/*.src $(BIN_DIR)/*.info
	
show:
	@echo SRCS = $(SRCS)
	@echo C_OBJECTS = $(C_OBJECTS)
	@echo C_OBJECTS_EXT = $(C_OBJECTS_EXT)
	@echo TARGET_ELF = $(TARGET_ELF)
