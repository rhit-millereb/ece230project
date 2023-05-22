/*
 * music.h
 *
 *  Created on: May 15, 2023
 *      Author: disalvtj and Ethan Miller
 */

#ifndef MUSIC_H_
#define MUSIC_H_

// TIMER_A0 used to generate notes
// TIMER_A1 used to hold notes/rests

//48MHz system clock and 4:1 prescaler
//12MHz Timer A clock
//Half period count = (48,000,000/Note frequency)
#define SMCLK 48000000 //Hz
#define TimerA0Prescaler 4 //Timer A prescaler
#define TimerA0Clock  SMCLK/TimerA0Prescaler

//rest note, one shot signal of 1/48MHz pulse width is generated,
//which is not audible
//This is not the inserted rest for 100ms when Speaker should remain low voltage.
#define RestNote 1

// NOTE FREQUENCIES
#define FrequencyF3s    185.00  // Lowest possible note given timer configurations!
#define FrequencyG3     196.00
#define FrequencyG3s    207.65
#define FrequencyA3     220.00
#define FrequencyA3s    233.08
#define FrequencyB3     246.94
#define FrequencyC4     261.63 // Middle C, C4
#define FrequencyC4s    277.18
#define FrequencyD4     293.66
#define FrequencyD4s    311.13
#define FrequencyE4     329.63
#define FrequencyF4     349.23
#define FrequencyF4s    369.99
#define FrequencyG4     392.00
#define FrequencyG4s    415.30
#define FrequencyA4     440.00  //Tuning Standard, A4
#define FrequencyA4s    466.16
#define FrequencyB4     493.88
#define FrequencyC5     523.25
#define FrequencyC5s    554.37
#define FrequencyD5     587.33
#define FrequencyD5s    622.25
#define FrequencyE5     659.25
#define FrequencyF5     698.46
#define FrequencyF5s    739.99
#define FrequencyG5     783.99
#define FrequencyG5s    830.61
#define FrequencyA5     880.00
#define FrequencyA5s    932.33
#define FrequencyB5     987.77
#define FrequencyC6     1046.50

// PLAYABLE NOTES
// Note A4 - 440 Hz, B4 - 493.88 Hz, C5 - 523.26 Hz
#define NOTEC3 TimerA0Clock/FrequencyC3
#define NOTEC3s TimerA0Clock/FrequencyC3s
#define NOTED3 TimerA0Clock/FrequencyD3
#define NOTED3s TimerA0Clock/FrequencyD3s
#define NOTEE3 TimerA0Clock/FrequencyE3
#define NOTEF3 TimerA0Clock/FrequencyF3
#define NOTEF3s TimerA0Clock/FrequencyF3s
#define NOTEG3  TimerA0Clock/FrequencyG3
#define NOTEG3s TimerA0Clock/FrequencyG3s
#define NOTEA3  TimerA0Clock/FrequencyA3
#define NOTEA3s TimerA0Clock/FrequencyA3s
#define NOTEB3  TimerA0Clock/FrequencyB3
#define NOTEC4  TimerA0Clock/FrequencyC4
#define NOTEC4s TimerA0Clock/FrequencyC4s
#define NOTED4  TimerA0Clock/FrequencyD4
#define NOTED4s TimerA0Clock/FrequencyD4s
#define NOTEE4  TimerA0Clock/FrequencyE4
#define NOTEF4  TimerA0Clock/FrequencyF4
#define NOTEF4s TimerA0Clock/FrequencyF4s
#define NOTEG4  TimerA0Clock/FrequencyG4
#define NOTEG4s TimerA0Clock/FrequencyG4s
#define NOTEA4  TimerA0Clock/FrequencyA4    // (48,000,000/440)=4545.45
#define NOTEA4s TimerA0Clock/FrequencyA4s
#define NOTEB4  TimerA0Clock/FrequencyB4    // 4049.57
#define NOTEC5  TimerA0Clock/FrequencyC5    // 3822.19
#define NOTEC5s TimerA0Clock/FrequencyC5s
#define NOTED5  TimerA0Clock/FrequencyD5
#define NOTED5s TimerA0Clock/FrequencyD5s
#define NOTEE5  TimerA0Clock/FrequencyE5
#define NOTEF5  TimerA0Clock/FrequencyF5
#define NOTEF5s TimerA0Clock/FrequencyF5s
#define NOTEG5  TimerA0Clock/FrequencyG5
#define NOTEG5s TimerA0Clock/FrequencyG5s
#define NOTEA5  TimerA0Clock/FrequencyA5
#define NOTEA5s TimerA0Clock/FrequencyA5s
#define NOTEB5  TimerA0Clock/FrequencyB5    // Speaker struggles going this low, so experiment to find limits later
#define NOTEC6  TimerA0Clock/FrequencyC6


// NOTE DURATIONS
#define ACLK            32768   //Hz
#define WHOLE_NOTE      ACLK/2    //1 second
#define HALF_NOTE       ACLK/4  //0.5 second
#define QUARTER_NOTE    ACLK/8  //0.25 second
#define EIGHTH_NOTE     ACLK/16  //0.125 second
#define DELAY100MS      ACLK/5 //100ms timer delay

// SAVED SONGS
// "Twinkle Tinkle Little Star (The Star)" by Jane Taylor
const uint16_t Song_TwinkleTwinkleLittleStar[] = {
                              NOTEG3, NOTEG3, NOTED4, NOTED4, NOTEE4, NOTEE4, NOTED4, RestNote,
                              NOTEC4, NOTEC4, NOTEB3, NOTEB3, NOTEA3, NOTEA3, NOTEG3, RestNote,
                              NOTED4, NOTED4, NOTEC4, NOTEC4, NOTEB3, NOTEB3, NOTEA3, RestNote,
                              NOTED4, NOTED4, NOTEC4, NOTEC4, NOTEB3, NOTEB3, NOTEA3, RestNote,
                              NOTEG3, NOTEG3, NOTED4, NOTED4, NOTEE4, NOTEE4, NOTED4, RestNote,
                              NOTEC4, NOTEC4, NOTEB3, NOTEB3, NOTEA3, NOTEA3, NOTEG3, RestNote,
                              RestNote, RestNote, RestNote, RestNote,
                              NULL};
const uint16_t Notes_TwinkleTwinkleLittleStar[] = {
                              QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,
                              QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,
                              QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,
                              QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,
                              QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,
                              QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,
                              QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,
                              NULL};
// "Hello, World" by Louie Zong
const uint16_t Song_HelloWorld[] = {
                              NOTEF4, NOTEC4, NOTEG4, NOTEG4, // Hello, world
                              NOTEA4, NOTEC5, NOTEA4, NOTEF4, // Programmed to work
                              NOTED4, NOTEC4, NOTED4, NOTEA4, NOTEG4, NOTEA4, NOTEA4, // and not to feel
                              NOTEA4, NOTEG4, NOTEF4, NOTEE4, NOTED4, // Not even sure that
                              NOTEA3, NOTEC4, NOTEE4, NOTEF4, NOTEE4, NOTEE4, // this is real
                              NOTEA4, NOTED4, NOTEF4, NOTEF4, // Hello, world.
                              NULL};
const uint16_t Notes_HelloWorld[] = {
                              QUARTER_NOTE, QUARTER_NOTE, WHOLE_NOTE, QUARTER_NOTE,
                              QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,
                              QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, QUARTER_NOTE, WHOLE_NOTE,
                              QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE, QUARTER_NOTE,
                              QUARTER_NOTE, QUARTER_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, QUARTER_NOTE, WHOLE_NOTE,
                              QUARTER_NOTE, QUARTER_NOTE, WHOLE_NOTE, HALF_NOTE,
                              NULL};

#endif /* MUSIC_H_ */
