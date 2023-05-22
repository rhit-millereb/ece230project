/*
 * potToDigConvert.h
 *
 *  Created on: Apr 24, 2023
 *      Author: millereb and Taylor DiSalvo
 */

#ifndef POTTODIGCONVERT_H_
#define POTTODIGCONVERT_H_


#define POT_PORT1 P5
#define POT1_BIT BIT4
#define POT2_BIT BIT5
#define POT_PORT2 P6
#define POT3_BIT BIT0

#define PHOTO_PORT P6
#define PHOTO_BIT BIT1
#define MEMORYCHANNEL 0x1

#include "msp.h"

static volatile double maxPOTValue = 4090;


/*
 * Set up the timers necessary for digital conversion
 */
extern void initConversion();


extern void enableConversion();


#endif /* POTTODIGCONVERT_H_ */
