/*************************************************************************
 * Projet Raspberry pi lcd controller
 *************************************************************************
 * File : pic16f876a_controller_fifo.c
 *************************************************************************
 * Date : 11 Octobre 2012
 * Author : Sinseman44
 * Organization : Coloc's bar
 *************************************************************************/

/*
 *                      -----------
 *  Read Pointer -----> |         |
 *                      -----------
 *                      |         | <------ Write Pointer
 *                      -----------
 *                      |         |
 *                      -----------
 *                      |         |
 *                      -----------
 */

/*************************************************************************
 * Inclusion(s)
 *************************************************************************/

#include "pic16f876a_controller_fifo.h"

/*************************************************************************
 * Constante(s)/Macro(s)
 *************************************************************************/

#define BUFFER_STAT_EMPTY		0 		/*!< Status buffer empty */
#define BUFFER_STAT_NOT_EMPTY	1 		/*!< Status buffer not empty */
#define BUFFER_STAT_FULL		2 		/*!< Status buffer full */
#define FIFO_MAX_SIZE			64 		/*!< Buffer size max */

uint8_t gb_flag_cur_state_buf = 0;		/*!< Flag to indicate buffer status */
uint8_t gpui8_buffer[FIFO_MAX_SIZE];	/*!< Static buffer */

/*************************************************************************
 * Prototype(s)
 *************************************************************************/

/*************************************************************************
 * Fonction(s)
 *************************************************************************/

/**
 * @fn void fifo_init(void)
 * @brief initialise buffer and the read and write pointers
 * @param none
 * @return none
 */
void fifo_init(void) {
	uint8_t ui8_idx = 0;
	// initialise buffer
	for(ui8_idx = 0; ui8_idx < FIFO_MAX_SIZE; ui8_idx ++) {
		gpui8_buffer[ui8_idx] = 0;
	}

	// initialise pointers
	gpui8_read_buf = &gpui8_buffer[0];
	gpui8_write_buf = &gpui8_buffer[0];
	gb_flag_cur_state_buf = BUFFER_STAT_EMPTY;
	
	//~ uart_printf("START pointer = 0x%x\n\0", gpui8_read_buf);
	return;
}

/**
 * @fn int8_t fifo_put(const uint8_t ui8_value)
 * @brief Save value to the buffer and increment pointer if it's possible
 *
 * @param [in] ui8_value	Value to save to the buffer
 * @return -1 if the buffer is full otherwise 0
 */
int8_t fifo_put(const uint8_t /* in */ ui8_value) {
	RB6 = 1;
	if(gb_flag_cur_state_buf == BUFFER_STAT_FULL) {
		return RET_FIFO_NOK;
	} else {
		*(gpui8_write_buf) = ui8_value;
		if(gpui8_write_buf == &gpui8_buffer[FIFO_MAX_SIZE - 1]) {
			// return to the start of the buffer
			gpui8_write_buf = &gpui8_buffer[0];
		}
		else {
			// increment the pointer
			gpui8_write_buf ++;
		}

		if(gpui8_write_buf == gpui8_read_buf) {
			gb_flag_cur_state_buf = BUFFER_STAT_FULL;
			return RET_FIFO_NOK;
		}
		else {
			gb_flag_cur_state_buf = BUFFER_STAT_NOT_EMPTY;
		}
	}
	RB6 = 0;
	return RET_FIFO_OK;
}

/**
 * @fn int8_t fifo_get(uint8_t * const pui8_value) 
 * @brief get a byte from the fifo and increment pointer if it's possible
 *
 * @param [out] pui8_value
 * @return RET_FIFO_NOK if an error occurs
 * 		   RET_FIFO_EMPTY if the fifo is empty
 * 		   RET_FIFO_OK otherwise
 */
int8_t fifo_get(uint8_t * /* out */ const pui8_value) {
	// test if value address is null
	if(pui8_value == (uint8_t *)NULL) {
		return RET_FIFO_NOK;
	}
	/* else nothing to do */

	// test if state buffer is previously empty
	if(gb_flag_cur_state_buf == BUFFER_STAT_EMPTY) {
		return RET_FIFO_EMPTY;
	}
	/* else nothing to do */

	// retreive buffered value
	*(pui8_value) = *(gpui8_read_buf);
	if(gpui8_read_buf != gpui8_write_buf) {
		if(gpui8_read_buf == &gpui8_buffer[FIFO_MAX_SIZE - 1]) {
			// return to the start of the buffer
			gpui8_read_buf = &gpui8_buffer[0];
		}
		else {
			// increment the pointer
			gpui8_read_buf ++;
		}

		// re-test if state changes
		if(gpui8_read_buf == gpui8_write_buf) {
			gb_flag_cur_state_buf = BUFFER_STAT_EMPTY;
			__delay_us(50);
			return RET_FIFO_OK;
		}
		/* else nothing to do */
	} else {
		gb_flag_cur_state_buf = BUFFER_STAT_EMPTY;
		__delay_us(50);
		return RET_FIFO_OK;
	}


	__delay_us(50);
	return RET_FIFO_OK;
}

/**
 * @fn uint8_t fifo_state(void) 
 * @brief retrieve current state of the buffer
 *
 * @param none
 * @return the current state of the buffer
 */
uint8_t fifo_state(void) {
	return gb_flag_cur_state_buf;
}
