# Makefile pour projet PIC16F
CC   = picc
CHIP = 16F876A
OUTPUT_DIR = output
OBJECT_DIR = objet
SOURCE_DIR = src
HEADER_DIR = inc
DOC_DIR = doc
CFLAGS = -I$(HEADER_DIR)
ERR_FILE = compilation.log
EXEC = raspi_lcd_controller

P_CODE_FILES = $(OBJECT_DIR)/alfano_pic_16f877a_serial.p1 \
	       $(OBJECT_DIR)/pic16f876a_controller_i2c.p1 \
	       $(OBJECT_DIR)/pic16f876a_controller_lcd.p1 \
	       $(OBJECT_DIR)/pic16f876a_controller_fifo.p1 \
	       $(OBJECT_DIR)/pic16f876a_controller_frame.p1 \
	       $(OBJECT_DIR)/pic16f876a_controller.p1

all: test_dirs $(EXEC)

$(EXEC): $(P_CODE_FILES)
	@$(CC) --chip=$(CHIP) -M -E+$(ERR_FILE) $^ --OUTDIR=$(OUTPUT_DIR) --OBJDIR=$(OBJECT_DIR) -O$(EXEC) --SUMMARY=psect --SUMMARY=class --TIME -Q
	@cat $(ERR_FILE)

$(OBJECT_DIR)/%.p1: $(SOURCE_DIR)/%.c
	@$(CC) --chip=$(CHIP) -E+$(ERR_FILE) --pass1 $< --OBJDIR=$(OBJECT_DIR) $(CFLAGS) -Q

test_dirs:
	@if [ ! -d $(OBJECT_DIR) ]; then \
		mkdir -p $(OBJECT_DIR); \
	fi
	
	@if [ ! -d $(OUTPUT_DIR) ]; then \
		mkdir -p $(OUTPUT_DIR); \
	fi

help:
	@$(CC) --help

doc:
	@if [ ! -d $(DOC_DIR) ]; then \
		mkdir -p $(DOC_DIR); \
	fi
	doxygen -s -g

clean:
	@rm -f $(ERR_FILE)	
	@rm -f funclist
	@rm -f $(OBJECT_DIR)/*
	@rm -f $(OUTPUT_DIR)/*
