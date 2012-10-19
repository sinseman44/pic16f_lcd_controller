/*************************************************************************
 * Projet raspberry pi lcd
 *************************************************************************
 * File : pic16f876a_controller.c
 *************************************************************************
 * Date : 05 Octobre 2012
 * Author : Sinseman44
 * Organization : Coloc's bar
 *************************************************************************/

/*                          PIC16F876A
 *                 ---------------------------
 *               --|(1) /MCLR        RB7 (28)|--
 *        LCD_D7 --|(2) RA0          RB6 (27)|--
 *        LCD_D6 --|(3) RA1          RB5 (26)|--
 *        LCD_D5 --|(4) RA2          RB4 (25)|--
 *        LCD_D4 --|(5) RA3          RB3 (24)|--
 *        LCD_RS --|(6) RA4          RB2 (23)|--
 *        LCD_EN --|(7) RA5          RB1 (22)|-- 
 *           0 V --|(8) VSS          RB0 (21)|--
 *               --|(9) OSC1         VDD (20)|-- +5 V
 *               --|(10) OSC2        VSS (19)|--  0 V
 *               --|(11) RC0         RC7 (18)|-- 
 *               --|(12) RC1         RC6 (17)|-- 
 *               --|(13) RC2         RC5 (16)|-- LED
 *      SCL Line --|(14) SCL         SDA (15)|-- SDA Line
 *                 ---------------------------
 */

/*************************************************************************
 * Inclusion(s)
 *************************************************************************/

#include "pic16f876a_controller_frame.h"
#include "pic16f876a_controller_i2c.h"

/*************************************************************************
 * Constante(s)
 *************************************************************************/

// macro used to program the configuration fuses
__CONFIG(CP_OFF & DEBUG_OFF & WRT_OFF & CPD_OFF & LVP_OFF & BOREN_OFF & PWRTE_ON & WDTE_OFF & FOSC_HS);

uint8_t gui8_recv_byte_idx = 0; /*!< */
uint8_t gui8_frame_recv_size = 0; /*!< */
uint8_t gui8_token = 0; /*!< number of frames received from I2C bus */

/**
 * @fn void interrupt ISR_handle(void)
 * @brief
 *
 * @param none
 * @return nothing
 */
void interrupt ISR_handle(void) {
	uint8_t ui8_slave_state = 0;
	uint8_t ui8_i2c_value = 0;
	if(SSPIF == 1) {
		SSPIF = 0; // Reset IRQ flag
		i2c_slave_state(&ui8_slave_state);
		if((ui8_slave_state == 1) || (ui8_slave_state == 3)) {
			i2c_slave_read(); // Read SSPBUF to clear BF bit but don't care about returned value
			gui8_recv_byte_idx = 0;
			gui8_frame_recv_size = 0;
		}
		else if(ui8_slave_state == 2) {
			ui8_i2c_value = i2c_slave_read(); // Read SSPBUF to clear BF bit and read data from master
			fifo_put(ui8_i2c_value);

			// copy frame size byte
			if(gui8_recv_byte_idx == 1) {
				gui8_frame_recv_size = ui8_i2c_value - 1;
			}
			/* else nothing to do */

			// indicate that is the last byte received
			if((gui8_frame_recv_size == gui8_recv_byte_idx) && 
			   (gui8_recv_byte_idx != 0)) {
				gui8_token ++; // increase the number of frames received
			}
			/* else nothing to do */

			gui8_recv_byte_idx ++;
		}
		/* else nothing to do */
		CKP = 1;
	}
	/* else nothing to do */
}

/**
 * @fn void timer0_init(void)
 * @brief 
 * @param none
 * @return none
 */
void timer0_init(void) {	
	/* Enable TMR0 overflow interrupts */
	T0IE = 1;
	T0IF = 0;
	/* Configure Timer0 as follows :
     	 * 	- Use the internal instruction clock
	 *	  as the source to the module
	 *	- Assign the Prescaler to the TMR0
	 * 	  Timer so that TMR0 increments at a 1:16
	 * 	  ratio with the internal instruction clock */
	OPTION_REG = 0b00000011;
	TMR0 = 131;   // Set the TMR0 register to 131 to have 1ms overflow timer
		      // Internal instruction cycle = 1 / [(Processor Frequency) / 4] = 1 / (8 MHz / 4) = 500nS
		      // TMR0 overflow = Internal instruction cycle x 28 (we must count the zero) = 500nS x 256 = 128µS
		      // resolution = 1ms / 500 ns = 2000
		      // 2000 / prescaler = 2000 / 16 = 125
		      // 256(FFh to 00h) - 125 = 131
}

/**
 * @fn void switches_init(void)
 * @brief
 * 
 * RB0/INT => RTC square signal
 * 
 * put RB0 as input
 * Activate All RB pullup internal resistors to avoid use external pullup
 * Activate RB0 external interrupt 
 * 
 * @param none
 * @return none
 */
void interrupt_init(void) {
	/* Enable Global interrupts */
	GIE = 1;
	
	/* Enables all unmasked peripheral interrupts */
	PEIE = 1;
}

/**
 * @fn void main(void)
 * @brief main process
 * @param none
 * @return none
 */
void main(void) {
	TRISC5 = 0;
	TRISB7 = 0;
	TRISB6 = 0;
	RC5 = 0;
	RB7 = 0;
	RB6 = 0;
	uint16_t ui16_blink = 0;
	uint8_t ui8_idx = 0;
	int8_t i8_ret = -1;
	uint8_t ui8_frame_id = 0;
	uint8_t ui8_frame_size = 0;
	uint8_t ui8_value = 0;
	
	// initialise uart
	uart_init();

	// initialise Fifo	
	fifo_init();

	// initialisation
	interrupt_init();

	// initialise I2C HW
	i2c_init();

	// initialise lcd HW
	frame_init();
	
	uart_printf("LCD 2x16 CONTROLLER TEST\n\0");
	
	while(1) {
		// test if frames are in the buffer
		if(gui8_token != 0) {
			
			//~ fifo_get(&ui8_frame_id);
			//~ fifo_get(&ui8_frame_size);
			//~ uart_printf("------\n\0");
			//~ uart_printf("id = 0x%x\n\0", ui8_frame_id);
			//~ uart_printf("size = 0x%x\n\0", ui8_frame_size);
			//~ 
			//~ for(ui8_idx = 0; ui8_idx < (ui8_frame_size - 2); ui8_idx ++) {
				//~ fifo_get(&ui8_value);
				//~ uart_printf("value = 0x%x\n\0", ui8_value);
			//~ }
			
			//~ uart_printf("read pointer = 0x%x\n\0", gpui8_read_buf);
			//~ uart_printf("write pointer = 0x%x\n\0", gpui8_write_buf);
			
			i8_ret = frame_decode_fifo(); // decode frame and execute actions
			/*if(i8_ret != RET_OK) {
				return;
			}*/
			//~ uart_printf("gui8_token = %d\n\0", gui8_token);
			gui8_token --; // decrease number of frames in the buffer
		}
		/* else nothing to do */
		
		if(ui16_blink == 65000) {
			if(RC5 == 1) {
				RC5 = 0;
			}
			else {
				RC5 = 1;
			}
			ui16_blink = 0;
		}
		else {
			ui16_blink ++;
		}
	}
}
