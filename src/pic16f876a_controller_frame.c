/*************************************************************************
 * Projet Raspberry pi lcd controller
 *************************************************************************
 * File : pic16f876a_controller_frame.c
 *************************************************************************
 * Date : 11 Octobre 2012
 * Author : Sinseman44
 * Organization : Coloc's bar
 *************************************************************************/

/*
 * Frames :
 * --------
 * 
 * ------------------------------//-------
 * | Frame Id | Frame Size | Frame datas |
 * ------------------------------//-------
 *
 *
 * Identifiants :
 * --------------
 * 0x01 : Clear Display
 * 0x02 : Return home
 * 0x03 : Set Cursor
 * 0x04 : Put character
 * 0x05 : Put string
 * 0x06 : Control Display
 *
 * Frame =>  Clear Display :
 * -------------------------
 *
 * ---------------
 * | 0x01 | 0x02 |
 * ---------------
 *
 * Frame => Return home :
 * ----------------------
 *
 * ---------------
 * | 0x02 | 0x02 |
 * ---------------
 *
 * Frame => Set Cursor :
 * ---------------------
 * Cursor Row => 1 to 2
 * Cursor Column => 1 to 16
 *
 * --------------------------------------------
 * | 0x03 | 0x04 | CURSOR_ROW | CURSOR_COLUMN |
 * --------------------------------------------
 *
 * Frame => Put Character :
 * ------------------------
 *
 * ---------------------------
 * | 0x04 | 0x03 | CHARACTER |
 * ---------------------------
 *
 * Frame => Put String :
 * ---------------------
 *
 * --------------------------------------------
 * | 0x05 | 0x02 + STRING SIZE | STRING BYTES |
 * --------------------------------------------
 * 
 * Frame => Control display :
 * --------------------------
 * Display : Control ON (1) / OFF (0)
 * Cursor : Control ON (1) / OFF (0)
 * Blink : Control ON (1) / OFF (0) 
 * ------------------------------------------
 * | 0x06 | 0x05 | Display | Cursor | Blink |
 * ------------------------------------------
 */

/*************************************************************************
 * Inclusion(s)
 *************************************************************************/

#include "pic16f876a_controller_frame.h"
#include "pic16f876a_controller_lcd.h"

/*************************************************************************
 * Constante(s)/Macro(s)
 *************************************************************************/

#define CLEAR_FRAME_SIZE	2
#define HOME_FRAME_SIZE		2
#define CURSOR_FRAME_SIZE	4
#define CONTROL_FRAME_SIZE	5

/*************************************************************************
 * Prototype(s)
 *************************************************************************/

void frame_set_error(const int8_t /* in */ i8_error); 

/*************************************************************************
 * Fonction(s)
 *************************************************************************/

/**
 * @fn void frame_init(void)
 * @brief
 * @param
 * @return
 */
void frame_init(void) {
	lcd_init();
}

/**
 * @fn int8_t frame_decode(uint8_t * const pui8_frame)
 * @brief
 * @param none
 * @return RET_NOK if an error occurs otherwise RET_OK
 */
int8_t frame_decode_fifo(void) {
	int8_t i8_ret = -1;
	uint8_t ui8_frame_id = 0;
	uint8_t ui8_frame_size = 0;
	uint8_t ui8_idx = 0;
	uint8_t pui8_value[3] = {0, 0, 0};
	
	i8_ret = fifo_get(&ui8_frame_id);
	if(i8_ret != RET_FIFO_OK) {
		frame_set_error(i8_ret);
		return i8_ret;
	}
	/* else nothing to do */

	i8_ret = fifo_get(&ui8_frame_size);
	if(i8_ret != RET_FIFO_OK) {
		frame_set_error(i8_ret);
		return i8_ret;
	}
	/* else nothing to do */

	switch(ui8_frame_id) {
		case CLEAR_DISPLAY:
			// test frame size 
			if(ui8_frame_size == CLEAR_FRAME_SIZE) {
				di();
				lcd_clear_display();
				ei();
			}
			/* else nothing to do */
			break;

		case RETURN_HOME:
			if(ui8_frame_size == HOME_FRAME_SIZE) {
				di();
				lcd_return_home();
				ei();
			}
			/* else nothing to do */
			break;

		case SET_CURSOR:
			if(ui8_frame_size == CURSOR_FRAME_SIZE) {
				i8_ret = fifo_get(&pui8_value[0]);
				if(i8_ret != RET_FIFO_OK) {
					frame_set_error(i8_ret);	
					return i8_ret;
				}
				/* else nothing to do */

				i8_ret = fifo_get(&pui8_value[1]);
				if(i8_ret != RET_FIFO_OK) {
					frame_set_error(i8_ret);
					return i8_ret;
				}
				/* else nothing to do */

				di();
				lcd_set_cursor(pui8_value[0], pui8_value[1]);
				ei();
			}
			/* else nothing to do */			
			break;

		case PUT_CHAR:
			if(ui8_frame_size == 3) {
				i8_ret = fifo_get(&pui8_value[0]);
				if(i8_ret != RET_OK) {
					frame_set_error(i8_ret);
					return i8_ret;
				}
				/* else nothing to do */
				di();
				lcd_put_char((char_t)pui8_value[0]);
				ei();
			}
			/* else nothing to do */
			break;

		case PUT_STRING:
			for(ui8_idx = 0; ui8_idx < (ui8_frame_size - 2); ui8_idx ++) {
				i8_ret = fifo_get(&pui8_value[0]);
				if(i8_ret != RET_OK) {
					frame_set_error(i8_ret);
					return i8_ret;
				}
				/* else nothing to do */
				di();
				lcd_put_char((char_t)pui8_value[0]);
				ei();
			}
			
			break;

		case CONTROL_DISPLAY:
			if(ui8_frame_size == CONTROL_FRAME_SIZE) {
				for(ui8_idx = 0; ui8_idx < (ui8_frame_size - 2); ui8_idx ++) {
					i8_ret = fifo_get(&pui8_value[ui8_idx]);
					if(i8_ret != RET_FIFO_OK) {
						frame_set_error(i8_ret);	
						return i8_ret;
					}
					/* else nothing to do */
				}
				
				di();
				lcd_set_control(pui8_value[0], pui8_value[1], pui8_value[2]);
				ei();
			}
			/* else nothing to do */	
			break;

		default:
			break;
	}
	
	return RET_OK;
}

/**
 * @fn void frame_set_error(const int8_t i8_error) 
 * @brief
 * @param
 * @return none
 */
void frame_set_error(const int8_t /* in */ i8_error) {
	lcd_clear_display();
	switch(i8_error) {
		case RET_FIFO_NOK:
			lcd_put_string(11, "ERROR VALUE");
			break;

		case RET_FIFO_EMPTY:
			lcd_put_string(15 ,"ERROR EMPTY BUF");
			break;

		default:
			break;
	}
	return;
}
