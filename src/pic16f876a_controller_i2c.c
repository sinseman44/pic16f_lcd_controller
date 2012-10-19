/*************************************************************************
 * Projet raspberry pi controller lcd
 *************************************************************************
 * File : pic16f876a_controller_i2c.c
 *************************************************************************
 * Date : 06 Octobre 2012
 * Author : Sinseman44
 * Organization : Coloc's bar
 *************************************************************************/

/* Communication between display (slave) and control (master) :
 * 
 * I2C in Slave Mode: 
 * ------------------
 * In slave mode the pic16f876a 
 * 
 * Address:
 * --------
 * Slave address (7 bits) : 0x06 => 0 0 0 0 1 1 0 R/W
 *
 * Process:
 * -------- 
 * The SSPIF bit may be polled in software or can be configured as an interrupt source. 
 * Each time the SSPIF bit is set, the I2C event must be identified by testing various bits in the SSPSTAT register.
 *
 * There are a total of five valid states for the SSP module after an I2C event; 
 * these are described below.
 *
 * State 1: I2C write operation, last byte was an address byte.
 * SSPSTAT bits: S = 1, D_A = 0, R_W = 0, BF = 1
 *
 * State 2: I2C write operation, last byte was a data byte.
 * After the address byte is sent for an I C write operation (State 1), the Master may write one or more data bytes
 * to the Slave device.
 * SSPSTAT bits: S = 1, D_A = 1, R_W = 0, BF = 1
 *
 * State 3: I2C read operation, last byte was an address byte.
 * The Master device on the bus has begun a new read operation by initiating a START or a RESTART condition on the bus, 
 * then sending the Slave I2C address byte.
 * SSPSTAT bits: S = 1, D_A = 0, R_W = 1
 *
 * State 4: I2C read operation, last byte was a data byte.
 * state 4 occurs each time the Master has previously read a byte of data from the Slave and wishes to read
 * another data byte.
 * SSPSTAT bits: S = 1, D_A = 1, R_W = 1, BF = 0
 *
 * State 5: Slave I2C logic reset by NACK from master.
 * SSPSTAT bits: S = 1, D_A = 1, BF = 0, CKP = 1
 *
 * Each time SSPBUF is read in the slave firmware, the user should check the SSPOV bit to ensure that no 
 * reception overflows have occurred. If the user reads SSPBUF to clear the BF bit, but does not clear the SSPOV bit, 
 * the next byte of data received will be loaded into SSPBUF but the module will not generate an ACK pulse. 
 *
 * 
 * After writing data to SSPBUF, the user should check the WCOL bit to ensure that a write collision did not occur.
 */

/*************************************************************************
 * Inclusion(s)
 *************************************************************************/

#include "pic16f876a_controller_i2c.h"

/*************************************************************************
 * Constante(s)
 *************************************************************************/

#define SCL				RC3
#define SDA				RC4

#define PIC16F876A_I2C_SLAVE_ADDR	0x76

#define I2C_STAT_MASK			0x2D
#define STATE_1				0x09
#define STATE_2				0x29
#define STATE_3				0x0C
#define STATE_4				0x2C

uint8_t gui8_slave_token = 0;

/*************************************************************************
 * Prototype(s)
 *************************************************************************/

/*************************************************************************
 * Fonction(s)
 *************************************************************************/

/**
 * @fn void i2c_wait_for_idle(void)
 * @brief Wait for i2c bus is in IDLE state
 * @param none
 * @return nothing
 */
void i2c_wait_for_idle(void) {
	uint8_t ui8_flag_progress = 0;
	/*
         * SSPCON2.SEN must be equal to 0
	 * SSPCON2.RSEN must be equal to 0
	 * SSPCON2.PEN must be equal to 0
	 * SSPCON2.RCEN must be equal to 0
	 * SSPCON2.ACKEN must be equal to 0
	 * SSPSTAT.R_W must be equal to 0
         */
	/* wait for transmit is not in progress */
	while(ui8_flag_progress == 0){
		if(((SSPCON2 & 0x1F) == 0x00) && (R_W == 0)) {
			ui8_flag_progress = 1;
		}
		/* else nothing to do */
	}
}

/**
 * @fn
 * @brief
 * @param
 * @return
 */
uint8_t i2c_slave_read(void) {
	return (SSPBUF);
}

/**
 * @fn int8_t i2c_slave_scan(void)
 * @brief poll on i2c bus to retrieve data(s) or send data(s) from/to raspberry pi
 * @param [in] none
 * @return RET_NOK if an error occurs during execution otherwise
 *         RET_OK
 */
int8_t i2c_slave_state(uint8_t * pui8_state) {
	// State 1: I2C master write operation (slave read), last byte was an address byte
	if((SSPSTAT & I2C_STAT_MASK) == STATE_1) {
		if(SSPOV == 1) { // clear SSPOV bit
			SSPOV = 0;
		}
		/* else nothing to do */

		*pui8_state = 1;
		gui8_slave_token = 1;
	}
	// State 2: I2C master write operation (slave read), last byte was a data byte.
	else if(((SSPSTAT & I2C_STAT_MASK) == STATE_2) && 
		(gui8_slave_token == 1)) {
		if(SSPOV == 1) {
			// clear SSPOV bit
			SSPOV = 0;
		}
		/* else nothing to do */
		
		*pui8_state = 2;
	}
	//State 3: I2C master read operation (slave write), last byte was an address byte
	else if((SSPSTAT & I2C_STAT_MASK) == STATE_3) {
		*pui8_state = 3;
	}
	// State 4: I2C master read operation (slave write), last byte was a data byte.
	else if((SSPSTAT & I2C_STAT_MASK) == STATE_4) {
		*pui8_state = 4;
		// clear WCOL bit
		WCOL = 0;
		do {
		} while(WCOL == 1);
		// release clock
	}
	else {}

	return RET_OK;
}

/**
 * @fn int8_t i2c_init(void)
 * @brief Initialise the I2C in slave mode
 * @param none
 * @return RET_OK if the initialisation is done otherwise
 * 		   RET_NOK if the initialisation has already been done.
 */
int8_t i2c_init(void) {
	// Synchronous Serial Port Disable bit
	SSPEN = 0;
		
	TRISC3 = 1;
	TRISC4 = 1;
	// Set RC3 as Input
	//SCL = 1;
	// Set RC4 as Input
	//SDA = 1;
			
	// Synchronous Serial Port Mode Select bits
	// 1111 => I2C Slave mode, 10-bit address with Start and Stop bit interrupts enabled
	// 1110 => I2C Slave mode, 7-bit address with Start and Stop bit interrupts enabled
	// 1011 => I2C Firmware Controlled Master mode (Slave Idle)
	// 1000 => I2C Master mode, clock = FOSC/(4 * (SSPADD + 1))
	// 0111 => I2C Slave mode, 10-bit address
	// 0110 => I2C Slave mode, 7-bit address
	//
	SSPM3 = 0;
	SSPM2 = 1;
	SSPM1 = 1;
	SSPM0 = 0;

	// Set the Slave address
	SSPADD = PIC16F876A_I2C_SLAVE_ADDR << 1;  // 7 bits address = Bits 7654 321 <7:1>
	
	// CKP: SCK Release Control bit
	CKP = 1;
	
	// Synchronous Serial Port Enable bit
	SSPEN = 1;
	
	SSPCON2 = 0x01;
		
	//SMP: Slew Rate Control bit
	SMP = 1;

	//enable I²C interrupt
	SSPIE = 1;
	
	// clear SSPIF interrupt flag
	SSPIF = 0;
		
	return RET_OK;
}

/**
 * @fn void i2c_set_slave_addr(const uint8_t ui8_slave_addr, 
 *			       const uint8_t ui8_flag_read_write)
 * @brief Set slave address and read/write bit
 * @param [in] ui8_slave_addr		slave addr (7 bits)
 * @param [in] ui8_flag_read_write	flag to determine if it's a read (1) or write (0) command
 * @return nothing
 */
void i2c_set_slave_addr(const uint8_t /* in */ ui8_slave_addr, 
			const uint8_t /* in */ ui8_flag_read_write) {
	// wait for the i2c bus is ready
	i2c_wait_for_idle();

	// read flag
	if(ui8_flag_read_write == 1) {
		SSPBUF = (ui8_slave_addr << 1) + 1;
	}
	else if(ui8_flag_read_write == 0) {
		SSPBUF = ui8_slave_addr << 1;
	}
	else {}
}

/**
 * @fn void i2c_wait_for_slave_ack(void)
 * @brief Wait for Ack bit from slave
 * @param none
 * @return nothing
 */ 
void i2c_wait_for_slave_ack(void) {
	uint8_t ui8_flag_progress = 1;
	uint8_t ui8_ackstat = -1;
	/* waits for the ninth bit is an ACK sent by slave */
	do {
		ui8_ackstat = ACKSTAT;
		if(ui8_ackstat == 0) {
			ui8_flag_progress = 0;
		}
		/* else nothing to do */
	} while(ui8_flag_progress == 1);
}

/**
 * @fn void i2c_set_start(void)
 * @brief set start bit for I2C communication
 *        only in master mode
 * @param none
 * @return nothing
 */
void i2c_set_start(void) {
	// wait for the i2c bus is ready
	i2c_wait_for_idle();

	// Initiate Start condition on SDA and SCL pins
	SEN = 1;
}

/**
 * @fn void i2c_set_restart(void)
 * @brief set restart bit for I2C communication
 *        only in master mode
 * @param none
 * @return nothing
 */
void i2c_set_restart(void) {
	// wait for the i2c bus is ready
	i2c_wait_for_idle();

	// Initiate Restart condition on SDA and SCL pins
	RSEN = 1;
}

/**
 * @fn void i2c_set_stop(void)
 * @brief set stop bit for I2C communication
 *        only in master mode
 * @param none
 * @return nothing
 */
void i2c_set_stop(void) {
	// wait for the i2c bus is ready
	i2c_wait_for_idle();

	// Initiate Stop condition on SDA and SCL pins
	PEN = 1;
}

/**
 * @fn void i2c_read_byte(const uint8_t ui8_ack,
 *			  uint8_t * const pui8_read_byte)
 * @brief read data from I2C
 * @param ui8_ack			send acknowledge (1 = True, 0 = False)
 * @param pui8_read_byte		data retrieve from i2c bus
 * @return nothing
 */
void i2c_read_byte(const uint8_t /* in */ ui8_ack,
		   uint8_t * const /* out */ pui8_read_byte) {
	// wait for the i2c bus is ready
	i2c_wait_for_idle();

	// set enable the reception of data from I2C
	// (only in master mode)
	RCEN = 1;

	// wait for the i2c bus is ready
	i2c_wait_for_idle();
	
	// read SSPBUF register in out parameter
	*pui8_read_byte = SSPBUF;

	if(ui8_ack == 1) {
		// initiate acknowledge
		ACKDT = 0;
	}
	else if(ui8_ack == 0) {
		// not initiate acknowledge
		ACKDT = 1;
	}
	/* else nothing to do */

	// send acknowledge sequence
	ACKEN = 1;
	return;
}

/**
 * @fn void i2c_write_byte(const uint8_t ui8_byte_to_write)
 * @brief write data to I2C
 * @param ui8_byte_to_write		data to write to I2C bus
 * @return nothing
 */
void i2c_write_byte(const uint8_t /* in */ ui8_byte_to_write) {
	// wait for the i2c bus is ready
	i2c_wait_for_idle();

	// put input parameter in SSPBUF register
	SSPBUF = ui8_byte_to_write;
	return;
}
