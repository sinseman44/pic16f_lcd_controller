# Makefile pour projet PIC16F
CC   = picc
CHIP = 16F876A
OUTPUT_DIR = output
OBJECT_DIR = objet
SOURCE_DIR = src
HEADER_DIR = inc
CFLAGS = -I$(HEADER_DIR)
ERR_FILE = compilation.log
EXEC = raspi_lcd_controller

P_CODE_FILES = $(OBJECT_DIR)/alfano_pic_16f877a_serial.p1 \
			$(OBJECT_DIR)/pic16f876a_controller_fifo.p1 \
	       $(OBJECT_DIR)/pic16f876a_controller_i2c.p1 \
	       $(OBJECT_DIR)/pic16f876a_controller_lcd.p1 \
	       $(OBJECT_DIR)/pic16f876a_controller_frame.p1 \
	       $(OBJECT_DIR)/pic16f876a_controller.p1

all: $(EXEC)

$(EXEC): $(P_CODE_FILES)
	@$(CC) --chip=$(CHIP) -M -E+$(ERR_FILE) $^ --OUTDIR=$(OUTPUT_DIR) --OBJDIR=$(OBJECT_DIR) -O$(EXEC) --SUMMARY=psect --SUMMARY=class --TIME -Q
	@cat $(ERR_FILE)

$(OBJECT_DIR)/%.p1: $(SOURCE_DIR)/%.c
	@$(CC) --chip=$(CHIP) -E+$(ERR_FILE) --pass1 $< --OBJDIR=$(OBJECT_DIR) $(CFLAGS) -Q

help:
	@$(CC) --help

clean:
	@rm $(ERR_FILE)
	@rm funclist
	@rm $(OBJECT_DIR)/*
	@rm $(OUTPUT_DIR)/*
	
	
