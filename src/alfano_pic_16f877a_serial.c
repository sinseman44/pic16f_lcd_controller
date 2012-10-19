/*************************************************************************
 * Projet Alfano Perso pour voiture/moto avec capteur effet hall
 *************************************************************************
 * File : alfano_pic_16f877a_serial.c
 *************************************************************************
 * Date : 12 Octobre 2011
 * Author : Sinseman44
 * Organization : Coloc's bar
 *************************************************************************/

 /*              VCC 
  *              ---
  *               |---------------------------------+        ____
  *          1uF ===            MAX232              |       5|   \
  *               |   -------------------------     |    +---| o  \
  *         +-----c---|(1) C1+        VCC (16)|-----+    |   |   o \ 
  *    1uF ===    +---|(2) Vs+        GND (15)|----------+  3| o   |
  *         +---------|(3) C1-      T1OUT (14)|----------c---|   o |
  *         +---------|(4) C2+       R1IN (13)|----------c---| o   | DB9
  *    1uF === +------|(5) C2-      R1OUT (12)|-- RC7    |  2|   o |
  *         +--+  +---|(6) VS-       T1IN (11)|-- RC6    |   | o   |
  *               | x-|(7) T2OUT     T2IN (10)|-x        |   |   o /
  *               | x-|(8) R2IN     R2OUT  (9)|-x        |   | o  /
  *               |   -------------------------          |   |   /
  *               |                                      |    ---
  *          1uF ===                                     |
  *               |--------------------------------------+
  *              ----
  *              ////
  */ 

/*************************************************************************
 * Inclusion(s)
 *************************************************************************/

#include "alfano_pic_16f877a_serial.h"

/*************************************************************************
 * Constante(s)
 *************************************************************************/

/*************************************************************************
 * Prototype(s)
 *************************************************************************/

/*
 * send a character in UART
 */ 
void uart_print_char(const char_t /* in */ i8_char);

/*
 * send a string in UART
 */ 
void uart_print_str(char_t * const /* in */ sz_string);

/*************************************************************************
 * function(s)
 *************************************************************************/
 
/**
 * @fn void uart_init(void)
 * @brief initialisation of uart communication
 * 
 * @param
 * @return
 */ 
void uart_init(void)
{
	BRGH = 1;	/* high baud rate */
	SPBRG = 51; /* Baud Rate = Fosc/(16 (X + 1)) with X = 9600 => Baud Rate = 51 */
	SYNC = 0;	/* asynchronous */
	SPEN = 1;	/* enable serial port pins */
	CREN = 1;	/* enable reception */
	RCIE = 1;	/* enable rx interrupts */
	RX9  = 0;	/* 8-bit reception */
	SREN = 0;	/* no effect */
	TXIE = 0;	/* disable tx interrupts */
	TX9  = 0;	/* 8-bit transmission */
	TXEN = 1;	/* enable the transmitter */
}

/**
 * @fn void serial_print_char(const char_t i8_char)
 * @brief
 * 
 * @param
 * @return
 */ 
void uart_print_char(const char_t /* in */ i8_char)
{
	/* wait for register is empty */
	while(TXIF == 0)
	{}
	
	TXREG = i8_char;
	
	while(TRMT == 1)
	{}
}

/**
 * @fn void serial_print_str(const char_t * const sz_string)
 * @brief
 * 
 * @param
 * @return
 */ 
void uart_print_str(char_t * const /* in */ sz_string)
{
	char_t * pi8_string = &sz_string[0];
	
	while(*pi8_string != '\0')
	{
		uart_print_char(*pi8_string);
		pi8_string ++;
	}
}

/**
 * @fn
 * @brief
 * 
 * @param
 * @return
 */ 
void uart_printf(char_t * const /* in */ sz_format, ...)
{
	char_t * pst_str = &sz_format[0];
	va_list pst_list;
	va_start(pst_list, sz_format);
	int32_t i32_var_int = 0;
	char_t  i8_var_char = 0x00;
	char_t sz_buf[20];
	char_t * pi8_buf = &sz_buf[0];
	char_t * sz_var_str;
	
	while(*pst_str != '\0')
    {
		if(*pst_str == '%')
		{
			switch(*(++pst_str)) 
			{
				case 's': /* chaîne */
					sz_var_str = va_arg(pst_list, char_t *);
					uart_print_str(sz_var_str);
					break;

				case 'd': /* entier */
					i32_var_int = 0;
					memset(pi8_buf, 0, sizeof(sz_buf));
					i32_var_int = va_arg(pst_list, int32_t);
					itoa(pi8_buf, i32_var_int, 10);
					uart_print_str(pi8_buf);
					break;

				case 'x': /* hexadecimal */
					i32_var_int = 0;
					memset(pi8_buf, 0, sizeof(sz_buf));
					i32_var_int = va_arg(pst_list, int32_t);
					itoa(pi8_buf, i32_var_int, 16);
					uart_print_str(pi8_buf);
					break;

				case 'c': /* caractère */
					i8_var_char = va_arg(pst_list, char_t);
					uart_print_char(i8_var_char);
					break;
				
				default:
					break;
			}
		}
		else
		{
			uart_print_char(*pst_str);
		}
		pst_str ++;
	}
	va_end(pst_list);
	
}
