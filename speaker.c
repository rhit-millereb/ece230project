/*
 * speaker.c
 *
 *  Created on: May 12, 2023
 *      Author: millereb and Taylor Disalvo
 */

#include "msp.h"
#include "speaker.h"

// TIMER_A0 used to generate notes
// TIMER32_1 used to generate notes, if used instead
// TIMER_A1 used to hold notes/rests

uint16_t currentNote = 0;
uint16_t totalNotes = 0;


void stop(void) {
    //set timer to stop mode
    TIMER_A0->CTL &= ~0b010000;
    TIMER_A1->CTL &= ~0b110000;

    //clear registers
    TIMER_A0->R &= 0x0000;
    TIMER_A1->R &= 0x0000;

    currentNote = 0;
}

void configureSoundTimer(void)
{

    // Set Period in CCR0 register
        TIMER_A0->CCR[0] = notePeriods[0] - 1;
        TIMER_A0->CCTL[1] |= 0x0060; //set the control for CCR1 (Compare, Set/Reset, No interrupt)
        TIMER_A0->CTL |= 0x0294; //set the TimerA0 control (SMCLK source, Up Mode, 4:1 prescale)
        TIMER_A0->CTL &= 0xFFCF; // set to stop mode
}

void configureIntervalTimer(void)
{
    // Set initial period in CCR2 register. This assumes timer starts at 0
    TIMER_A1->CCR[2] = noteLengths[0];
    // Configure CCR2 for Compare mode with interrupt enabled (no output mode - 0)
    TIMER_A1->CCTL[2] = 0x0010;

    // Configure Timer_A1 in Continuous Mode with source ACLK prescale 1:1 and interrupt enabled
    TIMER_A1->CTL = 0x0126;
    //set the timer to stop mode
    TIMER_A1->CTL &= 0xFFCF;

    // Enable TA1 overflow and TA1CCR2 compare interrupt
    NVIC->ISER[0] |= 0x0800;
}


void configureSpeaker(void)
{
    P2->DIR |= BIT4;            // set P2.4 as output
    P2->SEL0 |= BIT4;
    P2->SEL1 &= ~BIT4;

    noteCount = 0;


    configureSoundTimer();
    configureIntervalTimer();

    __enable_irq();
}

void play(void) {
    configureSoundTimer();
    configureIntervalTimer();

    //set timer to up mode
    TIMER_A0->CTL |= 0b010000;
    TIMER_A1->CTL |= 0b100000;
}


void setMusic(uint16_t periods[], uint16_t lengths[], uint16_t count) {
    totalNotes = count;
    int i = 0;
    for (i = 0; i < MAXNOTES; i++)
    {
        notePeriods[i] = periods[i];
        noteLengths[i] = lengths[i];
    }
}

void T32_INT1_IRQHandler(void) {
    TIMER32_1->INTCLR = 0; // Clear flag
    P2->OUT ^= BIT4; // Toggle the speaker output
}

void TA1_N_IRQHandler(void)
{

    /* Check if interrupt triggered by timer overflow */
    if (TIMER_A1->CTL & TIMER_A_CTL_IFG)
    {
        // Clear timer overflow flag
        // DONE clear flag in TA1CTL
        TIMER_A1->CTL = 0x0126;
    }
    /* Check if interrupt triggered by CCR2 */
    if (TIMER_A1->CCTL[2] & TIMER_A_CCTLN_CCIFG)
    {
        // Clear CCR2 compare interrupt flag
        TIMER_A1->CCTL[2] = 0x0010;

        currentNote++;

        if(currentNote>=totalNotes || notePeriods[currentNote] == 0) {
            stop();
        }

        //determine the next delay until next note
        TIMER_A1->CCR[2] += noteLengths[currentNote];

        uint16_t nextNote = notePeriods[currentNote];
        //set the next note
        TIMER_A0->CCR[0] = nextNote - 1;
//        TIMER32_1->LOAD = 4 * (nextNote / 2) - 1;
        // Set high pulse-width in CCR1 register (determines duty cycle)
        TIMER_A0->CCR[1] = (nextNote / 2) - 1;

    }
}

