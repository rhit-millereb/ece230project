/*
 * speaker.h
 *
 *  Created on: May 12, 2023
 *      Author: millereb and Talyor DiSalvo
 */

#ifdef __cplusplus
}
#endif

#ifndef SPEAKER_H_
#define SPEAKER_H_

#include <msp.h>

volatile uint16_t noteCount;
#define MAXNOTES 30

#define WHOLE_NOTE      32768 // Using LFXT at 32kHz: (32kHz/(1))*1 sec
#define HALF_NOTE       WHOLE_NOTE/2
#define QUARTER_NOTE    WHOLE_NOTE/4
#define EIGHTH_NOTE     WHOLE_NOTE/8  //0.125 second
#define DELAY100MS      WHOLE_NOTE/10 //interval rest is 100ms, or 10% of full note value

volatile uint16_t notePeriods[MAXNOTES];

volatile uint16_t noteLengths[MAXNOTES];


extern void configureSpeaker(void);


extern void play(void);

extern void stop(void);

extern void setMusic(uint16_t periods[], uint16_t lengths[], uint16_t count);

#endif /* SPEAKER_H_ */
