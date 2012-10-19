#ifndef PIC16F876A_CONTROLLER_INCLUDE
#define PIC16F876A_CONTROLLER_INCLUDE

/*************************************************************************
 * Projet Raspi LCD display
 *************************************************************************
 * File : alfano_pic_16f877a_include.h
 *************************************************************************
 * Date : 21 Octobre 2011
 * Author : Sinseman44
 * Organization : Coloc's bar
 *************************************************************************/

/*************************************************************************
 * Inclusion(s)
 *************************************************************************/

#include <htc.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

/*************************************************************************
 * Constante(s)
 *************************************************************************/
// FOSC = 8MHz

#define _XTAL_FREQ 				8000000

#define RET_OK					0
#define RET_NOK					-1


//#define __DEBUG__

typedef char 					char_t;
typedef signed char				int8_t;
typedef unsigned char 			uint8_t;
typedef signed short			short16_t;
typedef unsigned short			ushort16_t;
typedef signed int				int16_t;
typedef unsigned int 			uint16_t;
typedef signed short long 		int24_t;
typedef unsigned short long 	uint24_t;
typedef signed long 			int32_t;
typedef unsigned long 			uint32_t;

#endif /* PIC16F876A_CONTROLLER_INCLUDE */
