#ifndef PIC16F876A_CONTROLLER_LCD
#define PIC16F876A_CONTROLLER_LCD

/*************************************************************************
 * Projet Raspi LCD display
 *************************************************************************
 * File : pic16f876a_controller_lcd.h
 *************************************************************************
 * Date : 29 Septembre 2012
 * Author : Sinseman44
 * Organization : Coloc's bar
 *************************************************************************/

/*************************************************************************
 * Inclusion(s)
 *************************************************************************/

#include "pic16f876a_controller_include.h"

/*************************************************************************
 * Constante(s)
 *************************************************************************/

/*************************************************************************
 * Enuméré(s)
 *************************************************************************/ 

/*************************************************************************
 * Structure(s)
 *************************************************************************/ 

/*************************************************************************
 * Variable(s)
 *************************************************************************/

/*************************************************************************
 * Prototypes(s)
 *************************************************************************/

/*
 * initialisation of lcd
 */
int8_t lcd_init(void);

/* 
 * Clear and home the LCD 
 */
void lcd_clear_display(void);

/*
 * Return cursor to left
 */
void lcd_return_home(void);

/* 
 * Write a string of chars to the LCD 
 */
//~ int8_t lcd_put_string(const char_t * sz_string);
int8_t lcd_put_string(const uint8_t /* in */ ui8_str_size, 
		      const char_t * /* in */ sz_string);

/* 
 * Write one character to the LCD 
 */
void lcd_put_char(const char_t /* in */ i8_char);

/* 
 * Set the cursor position to the LCD screen 
 */
void lcd_set_cursor(const uint8_t /* in */ ui8_row, 
		    const uint8_t /* in */ ui8_column);

/*
 * Set Display control
 */
int8_t lcd_set_control(const uint8_t /* in */ ui8_display,
		       const uint8_t /* in */ ui8_cursor,
		       const uint8_t /* in */ ui8_blink);

#endif /* PIC16F876A_CONTROLLER_LCD */
