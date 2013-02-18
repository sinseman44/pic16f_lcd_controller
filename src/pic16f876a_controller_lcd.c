/*************************************************************************
 * Projet Alfano Perso pour voiture/moto avec capteur effet hall
 *************************************************************************
 * File : pic16f876a_controller_lcd.c
 *************************************************************************
 * Date : 05 Octobre 2012
 * Author : Sinseman44
 * Organization : Coloc's bar
 *************************************************************************/

 /* Address DDRAM :
  * 
  * 	    Col  Col  Col  Col  Col  Col  Col  Col  Col  Col  Col  Col  Col  Col  Col  Col
  *         1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16
  *       ---------------------------------------------------------------------------------
  * Row 1 | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | 0A | 0B | 0C | 0D | 0E | 0F |
  *       ---------------------------------------------------------------------------------
  * Row 2 | 40 | 41 | 42 | 43 | 44 | 45 | 46 | 47 | 48 | 49 | 4A | 4B | 4C | 4D | 4E | 4F |
  *       ---------------------------------------------------------------------------------
  *      
  *         
  *                    VCC
  *                    ---          LCD HD44780
  *                     |      +--------------------
  *       +-------------+      |
  *       |             |      |
  *      +-+     +------c------|(1)  VSS
  *  10k | |     |      +------|(2)  VDD
  *  Ohm | |<----u-------------|(3)  Contraste
  *      +-+     |       RA4 --|(4)  R/S
  *       |      +-------------|(5)  R/W
  *       |      |       RA5 --|(6)  EN
  *       |      +-------------|(7)  D0
  *       |      +-------------|(8)  D1
  *       |      +-------------|(9)  D2
  *       |      +-------------|(10) D3
  *       |      |       RA0 --|(11) D4
  *       |      |       RA1 --|(12) D5
  *       +------+       RA2 --|(13) D6
  *              |       RA3 --|(14) D7
  *              |       +-----|(15) A+
  *              +-------c-----|(16) A-
  *              |       |     |
  *              |      +-+    |
  *              |      | |100 +----------------------
  *            -----    | |Ohm
  *           /////     +-+
  *                      |
  *                     ---
  *                     VCC
  * 
  */                   

/*************************************************************************
 * Inclusion(s)
 *************************************************************************/

#include "pic16f876a_controller_lcd.h"

/*************************************************************************
 * Constante(s)/Macro(s)
 *************************************************************************/

#define	LCD_RS			RA4
#define LCD_EN 			RA5

#define LCD_D4			RA0
#define LCD_D5			RA1
#define LCD_D6			RA2
#define LCD_D7			RA3

/*************************************************************************
 * Prototype(s)
 *************************************************************************/

/* 
 * Write a byte to the LCD in 4 bit mode 
 */
int8_t lcd_write_byte(const uint8_t /* in */ ui8_byte);

/* 
 * Initialise custom characters 
 */
int8_t lcd_define_custom_char(const uint8_t /* in */ ui8_custom_char_size,
							  const char_t * /* in */ pi8_custom_char,
							  const uint8_t /* in */ ui8_addr_offset);

/* 
 * Load EEPROM data(s) in RAM 
 */
int8_t load_track_conf(const uint8_t /* in */ ui8_internal_eeprom_addr,
					   const uint8_t /* in */ ui8_size,
					   uint8_t * const /* out */ pui8_data);

/* */
void lcd_update_conf_tracks_lines(const uint8_t /* in */ ui8_first_line,
								  const uint8_t /* in */ ui8_cursor_line);
  
/* */
int8_t lcd_alf_set_track_name(const uint8_t /* in */ ui8_eeprom_addr,
							  const uint8_t /* in */ ui8_track_number);

/*************************************************************************
 * Fonction(s)
 *************************************************************************/

/**
 * @fn void lcd_write_byte(uint8_t ui8_char) 
 * 
 * @brief write a byte to the LCD in 4 bit mode 
 * 
 * @param [in] ui8_char		byte to write to the bus
 * @return RET_NOK if an error occurs during execution otherwise
 *         RET_OK
 */
int8_t lcd_write_byte(const uint8_t /* in */ ui8_byte) {
	LCD_D4 = (ui8_byte >> 4) & 0x01;
	LCD_D5 = (ui8_byte >> 5) & 0x01;
	LCD_D6 = (ui8_byte >> 6) & 0x01;
	LCD_D7 = (ui8_byte >> 7) & 0x01;
	
	LCD_EN = 1; 
	NOP();
	LCD_EN = 0;
	if(LCD_RS == 1) {
		__delay_us(200);
	}
	else {
		__delay_ms(5);
	}
	LCD_D4 = ui8_byte & 0x01;
	LCD_D5 = (ui8_byte >> 1) & 0x01;
	LCD_D6 = (ui8_byte >> 2) & 0x01;
	LCD_D7 = (ui8_byte >> 3) & 0x01;
	
	LCD_EN = 1; 
	NOP();
	LCD_EN = 0;
	if(LCD_RS == 1) {
		__delay_us(200);
	}
	else {
		__delay_ms(5);
	}
	return RET_OK;
}

/**
 * @fn void lcd_clear_display(void)
 * 
 * @brief Clear and home the LCD
 * 
 * @param none
 * @return nothing
 */
void lcd_clear_display(void) {
	LCD_RS = 0;
	lcd_write_byte(0x01);
}

/**
 * @fn void lcd_return_home(void) 
 * @brief Return Cursor to top left
 *
 * @param none
 * @return none
 */
void lcd_return_home(void) {
	LCD_RS = 0;
	lcd_write_byte(0x02);
}

/**
 * @fn void lcd_put_string(const char_t * sz_string)
 * 
 * @brief write a string of chars to the LCD 
 * 
 * @param [in] sz_string	string to write to the LCD
 * @return RET_NOK if an error occurs during execution otherwise
 *         RET_OK
 */
int8_t lcd_put_string(const uint8_t /* in */ ui8_str_size, 
		      const char_t * /* in */ sz_string) {
	uint8_t ui8_idx = 0;
	if(sz_string != (char_t *)NULL) {
		LCD_RS = 1;	// write characters
		while(ui8_idx < ui8_str_size) {
			lcd_write_byte(*sz_string++);
			ui8_idx ++;
		}
		LCD_RS = 0;
		return RET_OK;
	}
	else {
		return RET_NOK;
	}
}

/** 
 * @fn void lcd_put_char(const char_t i8_char)
 * 
 * @brief write one character to the LCD 
 * 
 * @param [in] i8_char 	character to write to LCD
 * @return nothing
 */
void lcd_put_char(const char_t /* in */ i8_char) {
	LCD_RS = 1;	// write character
	lcd_write_byte(i8_char);
	LCD_RS = 0;
}

/**
 * @fn int8_t lcd_set_control(const uint8_t ui8_display,
 *		       	      const uint8_t ui8_cursor,
 *		              const uint8_t ui8_blink) 
 * @brief
 *
 * @param [in] ui8_display
 * @param [in] ui8_cursor
 * @param [in] ui8_blink
 * @return 
 */
int8_t lcd_set_control(const uint8_t /* in */ ui8_display,
		       const uint8_t /* in */ ui8_cursor,
		       const uint8_t /* in */ ui8_blink) {
	uint8_t ui8_command = 0x08;
	if((ui8_display > 1) || 
	   (ui8_cursor > 1) || 
	   (ui8_blink > 1)) {
		return RET_NOK;
	}
	/* else nothing to do */
	
	LCD_RS = 0;
	if(ui8_display == 1) {
		ui8_command += 0x04;
	}
	/* else nothing to do */

	if(ui8_cursor == 1) {
		ui8_command += 0x02;
	}
	/* else nothing to do */

	if(ui8_cursor == 1) {
		ui8_command += 0x01;
	}
	/* else nothing to do */
	
	lcd_write_byte(ui8_command);
	return RET_OK;
}

/**
 * @fn int lcd_set_cursor(const unsigned char ui8_row, 
 *                        const unsigned char ui8_column)
 * @brief 
 * 
 * @param [in] ui8_row
 * @param [in] ui8_column
 * @return nothing
 */
void lcd_set_cursor(const uint8_t /* in */ ui8_row, 
		    const uint8_t /* in */ ui8_column) {
	uint8_t ui8_command = 0x80;
	LCD_RS = 0;
	switch(ui8_row) {
		case 1:
			ui8_command += ui8_column - 1;
			break;
			
		case 2:
			ui8_command += 0x40 + (ui8_column - 1);
			break;
			
		default:
			break;
	}
	
	lcd_write_byte(ui8_command);
}

/**
 * @fn void lcd_define_custom_char(void)
 * @brief initialise custom characters
 * 
 * There is enough character RAM for up to 8 custom characters to be defined
 * The character RAM pointer will auto-incriment
 * 
 * @param [in] ui8_custom_char_size
 * @param [in] pi8_custom_char
 * @param [in] ui8_addr_offset
 * @return RET_NOK if an error occurs during execution otherwise
 *         RET_OK
 */ 
int8_t lcd_define_custom_char(const uint8_t /* in */ ui8_custom_char_size,
			      const char_t * /* in */ pi8_custom_char,
			      const uint8_t /* in */ ui8_addr_offset) {
	uint8_t ui8_idx = 0;
	int8_t i8_ret = 0;
	if((ui8_custom_char_size > 0) && 
	   (ui8_addr_offset < 8) && 
	   (pi8_custom_char != (char_t *)NULL)) {
		LCD_RS = 0;
		// Send the Command (0x40 + address)
		i8_ret = lcd_write_byte(0x40 + ui8_addr_offset);
		if(i8_ret == RET_NOK) {
			return RET_NOK;
		}
		/* else nothing to do */
		// write character
		LCD_RS = 1;
		// Send the Data 
		for (ui8_idx = 0; ui8_idx < ui8_custom_char_size; ui8_idx++) {
			lcd_write_byte(*pi8_custom_char++);
			__delay_ms(1);
		}
		LCD_RS = 0;
		return RET_OK;
	}
	else {
		return RET_NOK;
	}
}

/**
 * @fn void load_track_conf(void)
 * @brief load EEPROM datas in RAM
 * 
 * @param [in] ui8_internal_eeprom_addr
 * @param [in] ui8_size
 * @param [out] pui8_data
 * @return RET_NOK if an error occurs during execution otherwise
 *         RET_OK
 */
int8_t load_track_conf(const uint8_t /* in */ ui8_internal_eeprom_addr,
                       const uint8_t /* in */ ui8_size,
		       uint8_t * const /* out */ pui8_data) {
	uint8_t ui8_idx = 0;
	uint8_t * pui8_data_ptr = pui8_data;
	
	if((pui8_data != (uint8_t *)NULL) && 
	   (ui8_size <= 16) &&
	   (ui8_internal_eeprom_addr < 0xFF)) {
		for(ui8_idx = 0; ui8_idx < ui8_size; ui8_idx ++) {
			*pui8_data_ptr = eeprom_read(ui8_internal_eeprom_addr + ui8_idx);
			pui8_data_ptr ++;
		}
		return RET_OK;
	}
	else {
		return RET_NOK;
	}
} 

/**
 * @fn void lcd_erase_internal_mem(void)
 * @brief Erase all the internal eeprom
 * @param none
 * @return nothing
 */ 
void lcd_erase_internal_mem(void) {
	uint8_t ui8_idx = 0;
	for(ui8_idx = 0; ui8_idx < 255; ui8_idx++) {
		eeprom_write(ui8_idx, 0xFF);
	}
}

/**
 * @fn int8_t lcd_set_logo_m2g(void)
 * @brief
 * @param none
 * @return RET_NOK if an error occurs during execution otherwise
 *         RET_OK
 */ 
int8_t lcd_set_logo_m2g(void) {
	int8_t i8_ret = 0;
	
	lcd_set_cursor(1, 1);
	i8_ret = lcd_put_string(13, "  COLOC'S BAR");
	if(i8_ret == RET_NOK){
		return i8_ret;
	}
	/* else nothing to do */
	lcd_set_cursor(2, 1); 
	i8_ret = lcd_put_string(14, "  RASPBERRY PI");
	if(i8_ret == RET_NOK){
		return i8_ret;
	}
	/* else nothing to do */
	__delay_ms(2500);
	
	//lcd_clear_display();
	
	return RET_OK;
}

/**
 * @fn void lcd_init(void)
 * @brief initialize LCD driver
 * 
 * @param none
 * @return RET_NOK if an error occurs during execution otherwise
 *         RET_OK
 */ 
int8_t lcd_init(void) {
	int8_t i8_ret = 0;
	
	ADCON1 = 0x06;
	// make the corresponding PORTA pin an output
	TRISA = 0;
	// clear all output pins
	PORTA = 0;
	
	// clear all three output pins
	LCD_RS = 0;
	LCD_EN = 0;
	
	// Wait for more than 15 ms after VCC rises to 4.5 V
	__delay_ms(15);
	// RS R/W DB7 DB6 DB5 DB4
	// 0   0   0   0   1   1
	LCD_D4 = 1;
	LCD_D5 = 1;
	LCD_D6 = 0;
	LCD_D7 = 0;
	// Il faut toujours envoyer une impulsion positive d'au moins 450ns, 
	// après la mise à l'état haut des broches DB5 et DB4, sur la broche EN.
	LCD_EN = 1; 
	NOP();
	LCD_EN = 0;
	// Wait for more than 4.1 ms
	__delay_ms(5);
	LCD_EN = 1;
	NOP(); 
	LCD_EN = 0;
	// Wait for more than 100 μs
	__delay_us(200);
	LCD_EN = 1;
	NOP(); 
	LCD_EN = 0;
	__delay_us(200);
	// RS R/W DB7 DB6 DB5 DB4 
	// 0   0   0   0   1   0  => interface four bits mode 
	LCD_D4 = 0;
	LCD_D5 = 1;
	LCD_D6 = 0;
	LCD_D7 = 0;
	
	LCD_EN = 1;
	NOP();
	LCD_EN = 0;

	__delay_ms(5);
	// => Set interface length
	// RS R/W DB7 DB6 DB5 DB4 
	// 0   0   0   0   1   0  
	// 0   0   N   F   *   * 
	lcd_write_byte(0x28);
	// => Display On, Cursor On, Cursor Blink Off
	// RS R/W DB7 DB6 DB5 DB4 
	// 0   0   0   0   0   0  
	// 0   0   1   0   0   0
	lcd_write_byte(0x08);
	// => Clear screen
	// RS R/W DB7 DB6 DB5 DB4 
	// 0   0   0   0   0   0  
	// 0   0   0   0   0   1
	lcd_write_byte(0x01);
	// => Set entry Mode
	// RS R/W DB7 DB6 DB5 DB4 
	// 0   0   0   0   0   0  
	// 0   0   0   1   D   S
	lcd_write_byte(0x06);
	// RS R/W DB7 DB6 DB5 DB4 
	// 0   0   0   0   0   0  
	// 0   0   1   D   C   B
	lcd_write_byte(0x0C);

	// erase display
	lcd_clear_display();
	
	lcd_set_logo_m2g();
	return i8_ret;
}
