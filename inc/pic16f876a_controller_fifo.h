#ifndef PIC16F876A_CONTROLLER_FIFO
#define PIC16F876A_CONTROLLER_FIFO

/*************************************************************************
 * Projet Raspi LCD display
 *************************************************************************
 * File : pic16f876a_controller_fifo.h
 *************************************************************************
 * Date : 12 octobre 2012
 * Author : Sinseman44
 * Organization : Coloc's bar
 *************************************************************************/

/*************************************************************************
 * Inclusion(s)
 *************************************************************************/

#include "pic16f876a_controller_include.h"
//~ #include "alfano_pic_16f877a_serial.h"

/*************************************************************************
 * Constante(s)
 *************************************************************************/

#define RET_FIFO_OK			 0
#define RET_FIFO_NOK		-1
#define RET_FIFO_EMPTY		-2

/*************************************************************************
 * Enuméré(s)
 *************************************************************************/ 

/*************************************************************************
 * Structure(s)
 *************************************************************************/ 

/*************************************************************************
 * Variable(s)
 *************************************************************************/

uint8_t *gpui8_read_buf = NULL;			/*!< Read pointer on the buffer */
uint8_t *gpui8_write_buf = NULL;		/*!< Write pointer on the buffer */

/*************************************************************************
 * Prototypes(s)
 *************************************************************************/

void fifo_init(void);

int8_t fifo_put(const uint8_t /* in */ ui8_value);

int8_t fifo_get(uint8_t * /* out */ const pui8_value);

uint8_t fifo_state(void);

#endif /* PIC16F876A_CONTROLLER_FIFO */
