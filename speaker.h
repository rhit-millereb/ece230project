/*
 * speaker.h
 *
 *  Created on: May 12, 2023
 *      Author: millereb
 */

#ifdef __cplusplus
}
#endif

#ifndef SPEAKER_H_
#define SPEAKER_H_

#include <msp.h>

volatile uint16_t noteCount;

#define FULL_NOTE       32768 // Using LFXT at 32kHz: (32kHz/(1))*1 sec
#define HALF_NOTE       FULL_NOTE/2
#define QUARTER_NOTE    FULL_NOTE/4
#define INTERVAL_REST   FULL_NOTE/10 //interval rest is 100ms, or 10% of full note value

volatile uint16_t notePeriods[10];

volatile uint16_t noteLengths[10];


extern void configureSpeaker(void);


extern void play(void);

extern void stop(void);

extern void setMusic(uint16_t periods[], uint16_t lengths[], uint16_t count);

#endif /* SPEAKER_H_ */
