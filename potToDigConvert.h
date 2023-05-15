/*
 * potToDigConvert.h
 *
 *  Created on: Apr 24, 2023
 *      Author: millereb
 */

#ifndef POTTODIGCONVERT_H_
#define POTTODIGCONVERT_H_


#define POT_PORT P5
#define POT_BIT BIT4
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
