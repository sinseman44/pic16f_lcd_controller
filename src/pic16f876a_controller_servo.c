/*************************************************************************
 * Projet Raspberry pi servo RC motor controller
 *************************************************************************
 * File : pic16f876a_controller_servo.c
 *************************************************************************
 * Date : 18 Octobre 2012
 * Author : Sinseman44
 * Organization : Coloc's bar
 *************************************************************************/

/*
 *                |
 * MAX_VALUE = 25 |_________________________________________
 *                |         /|         /|         /|
 * TOP_VALUE_CCW _|_ __  __/ |        / |        / |
 *                |       /  |       /  |       /  |
 *                |      /   |      /   |      /   |
 * TOP_VALUE_MID _|__ __/    |     /    |     /    |
 *                |    /     |    /     |    /     |
 *                |   /      |   /      |   /      |
 * TOP_VALUE_CW  _|__/       |  /       |  /       |
 *                | /        | /        | /        |
 *                |/         |/         |/         |
 *                +---------------------------------------->
 * PWM pin        |        __       ____     ______
 *                |       |  |     |    |   |      |
 *                |       |  |     |    |   |      |
 *                |       |  |     |    |   |      |
 *                |       |  |     |    |   |      |
 *                |       |  |     |    |   |      |
 *                |       |  |     |    |   |      |
 *                |       |  |     |    |   |      |
 *                |       |  |     |    |   |      |
 *                +-------+--+-----+----+---+------+------->
 *                        <-->     <---->   <------>
 *                        1 ms     1,5 ms     2 ms
 *                 <---------><---------><--------->
 *                    2,5 ms     2,5 ms     2,5 ms
 */

/*************************************************************************
 * Inclusion(s)
 *************************************************************************/

#include "pic16f876a_controller_servo.h"

/*************************************************************************
 * Constante(s)/Macro(s)
 *************************************************************************/

#ifdef __400_HZ__
	#define MAX_SERVO_PERIOD	25	/*!< 25 * 0,1 ms = 2,5 ms (400 Hz) */
	#define TOP_CCW_SERVO_VALUE	15	/*!< 10 (25 - 15) * 0.1 ms = 1 ms */
	#define TOP_CW_SERVO_VALUE	5	/*!< 20 (25 - 5) * 0.1 ms = 2 ms */ 
	#define TOP_MID_SERVO_VALUE 	10	/*!< 15 (25 - 10) * 0.1 ms = 1,5 ms */
#elif __300_HZ__
	#define MAX_SERVO_PERIOD	33	/*!< 33 * 0,1 ms = 3,3 ms (300 Hz) */
	#define TOP_CCW_SERVO_VALUE	23	/*!< 10 (33 - 23) * 0.1 ms = 1 ms */
	#define TOP_CW_SERVO_VALUE	13	/*!< 20 (33 - 13) * 0.1 ms = 2 ms */ 
	#define TOP_MID_SERVO_VALUE 	18	/*!< 15 (33 - 18) * 0.1 ms = 1,5 ms */
#elif __50_HZ__
	#define MAX_SERVO_PERIOD	200	/*!< 200 * 0,1 ms = 20 ms (50 Hz) */
	#define TOP_CCW_SERVO_VALUE	190	/*!< 10 (200 - 190) * 0.1 ms = 1 ms */
	#define TOP_CW_SERVO_VALUE	180	/*!< 20 (200 - 180) * 0.1 ms = 2 ms */ 
	#define TOP_MID_SERVO_VALUE 	185	/*!< 15 (200 - 185) * 0.1 ms = 1,5 ms */
#endif

#define SERVO_PWM_PIN		RB5	/*!< Pin use to emulate PWM with Timer0 */

/*************************************************************************
 * Prototype(s)
 *************************************************************************/

/*************************************************************************
 * Fonction(s)
 *************************************************************************/

/**
 * @fn void servo_init(void)
 * @brief 
 *
 * formula to calculate value of TMR0 :
 * ------------------------------------
 * Timer 0 period (sec) = [(TMR0 + 1)] * 4 * Tosc * (TMR0 Prescaler)
 *
 *
 * other method :
 * --------------
 * Internal instruction cycle = 1 / [(Processor Frequency) / 4]
 * resolution = Timer 0 period (sec) / Internal Instruction cycle (sec)
 * tmp_value = resolution / prescaler
 * TMR0 (256 (FFh to 00h)) = 256 - tmp_value
 *
 * For this project :
 * Internal instruction cycle = 1 / [8000000) / 4] = 500 ns
 * resolution = 0,0001 / 0,0000005 = 200
 * tmp_value = 200 / 2 = 100
 * TMR0 = 256 - 100 = 156
 *
 * @param
 * @return
 */
void servo_init(void) {
	TRISB5 = 0;		 // Define RB5 as Output
	SERVO_PWM_PIN = 0;	 // Set Pin as Low
	OPTION_REG = 0b00000000; // Timer TMR0 increments at a 1:2
	TMR0 = 156;          	 // Interupt every 0.1 ms
	T0IE = 1;	     	 // Enable interrupt on TMR0 overflow
	T0IF = 0;		 // Reset IRQ Timer0 flag
	return;
}

