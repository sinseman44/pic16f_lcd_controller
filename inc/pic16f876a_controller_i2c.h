#ifndef PIC16F876A_CONTROLLER_I2C
#define PIC16F876A_CONTROLLER_I2C

/*************************************************************************
 * Projet Raspi LCD display
 *************************************************************************
 * File : pic16f876a_controller_i2c.h
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
 * Enum(s)
 *************************************************************************/

enum {
	no_ack = 0,
	ack
};

/*************************************************************************
 * Prototype(s)
 *************************************************************************/

/*
 *
 */
uint8_t i2c_slave_read(void);

/*
 * 
 */ 
//~ int8_t i2c_slave_scan(void);
int8_t i2c_slave_state(uint8_t * pui8_state);

/*
 * Initialise the I2C in slave mode
 */
int8_t i2c_init(void);

/*
 * Set slave address and read/write bit
 */
void i2c_set_slave_addr(const uint8_t /* in */ ui8_slave_addr, 
		        const uint8_t /* in */ ui8_flag_read_write);

/*
 * Wait for Ack from slave
 */ 
void i2c_wait_for_slave_ack(void);

/*
 * Set start bit for I2C communication
 * only in master mode
 */
void i2c_set_start(void);

/*
 * Set restart bit for I2C communication
 * only in master mode
 */
void i2c_set_restart(void);

/*
 * Set stop bit for I2C communication
 * only in master mode
 */
void i2c_set_stop(void);

/*
 * Read data from I2C in master mode
 */
void i2c_read_byte(const uint8_t /* in */ ui8_ack,
		   uint8_t * const /* out */ pui8_read_byte);

/*
 * Write data to I2C in master mode
 */
void i2c_write_byte(const uint8_t /* in */ ui8_byte_to_write);

#endif /* PIC16F876A_CONTROLLER_I2C */
