


#include "msp.h"
#include <stdio.h>  //printf(); sprintf();
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "csHFXT.h"
#include "csLFXT.h"

#include "lcd.h"
#include "speaker.h"
#include "servoDriver.h"
#include "potToDigConvert.h"

uint16_t mainNoteLengths[10];
uint16_t mainNotePeriods[10];

uint16_t string1note;
uint16_t string2note;
uint16_t string3note;

char lcdMessage[200];

char lcdSetting; // 1 = showing string values, 2 = showing saved songs, 3 = welcome message
uint16_t lcdTime;

#define  Frequency10Hz 3200  // 50ms*32/ms = 1600

void sendLCDMessage(char *message) {
    clearDisplay();
    setLineNumber(0x00);

    int i = 0;
    while (message[i] != 0) {
        if (message[i] == '/') {
            setLineNumber(0x40);
        } else {
            printChar(message[i]);
        }

        i++;
    }
}

void configureSwitches(void) {
    //configure control switches
    P1->SEL0 &= BIT5+BIT6+BIT7;
    P1->SEL1 &= BIT5+BIT6+BIT7;
    P1->OUT |= BIT5+BIT6+BIT7;
    P1->REN |= BIT5+BIT6+BIT7;


}

bool playSwitchPressed(void) {
    char switchStatus = (P1->IN >> 5) & 1;
    if (switchStatus == 1) {
        //button is not pressed
        return false;
    } else {
        //button is pressed
        return true;
    }
}

bool stopSwitchPressed(void)
{
    char switchStatus = (P1->IN >> 6) & 1;
    if (switchStatus == 1) {
        //button is not pressed
        return false;
    }
    else {
        //button is pressed
        return true;
    }
}

bool saveSwitchPressed(void)
{
    char switchStatus = (P1->IN >> 7) & 1;
    if (switchStatus == 1) {
        //button is not pressed
        return false;
    }
    else {
        //button is pressed
        return true;
    }
}

void ConfigureTimerA0CCROInterrupt(void) {

    /* Configure Timer_A1 and CCRs */
     // Set initial period in CCR0 register. This assumes timer starts at 0
     TIMER_A2->CCR[0] = Frequency10Hz;
     // Configure CCR0 for Compare mode with interrupt enabled (no output mode - 0)
     TIMER_A2->CCTL[0]=0x0010;   //0b0000 0000 0001 CCIE=1 0000
     // Configure Timer_A1 in UP Mode with source ACLK prescale 1:1 and no interrupt
     TIMER_A2->CTL=0x0112;  //0b0000 0001 ACLK 0001 UP 0100

     /* Configure global interrupts and NVIC */
     NVIC->ISER[0] |= (1)<<TA2_0_IRQn;

     __enable_irq();
}

void mainLCD() {
    //display welcome message
    if(lcdSetting == '3') {
        //increase the timer value
        lcdTime++;

        if(lcdTime > 50) {
            //change mode to regular string display
            lcdSetting = '1';
        } else {
            sprintf(lcdMessage, "Welcome!/Play Some Music!");
            sendLCDMessage(lcdMessage);
        }
    }

    //standard display showing note values
    if(lcdSetting == '1') {
        sprintf(lcdMessage, "1  2  3/A  B  C");
        sendLCDMessage(lcdMessage);
    }
}

/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	/*
	 * INITIALIZE ALL HARDWARE
	 */

    //configure LCD
    configLCD(48000000);
    initLCD();

	//configure timers
	configHFXT();
	configLFXT();


	lcdTime = 0;
	lcdSetting = '3'; // start LCD in welcome mode

	configureSpeaker();

	ConfigureTimerA0CCROInterrupt();

	initConversion();

	configureSwitches();

	mainNoteLengths[0] = 35000;
	mainNotePeriods[0] = 50000;

	mainNoteLengths[1] = 5000;
	mainNotePeriods[1] = 40000;

	mainNoteLengths[2] = 5000;
	mainNotePeriods[2] = 30000;

	setMusic(mainNotePeriods, mainNoteLengths, 3);
	play();

	/*
	 * MAIN LOOP
	 */
	while(1) {
	    while(playSwitchPressed()) {}
	}

}

void TA2_0_IRQHandler(void)
{
    if (TIMER_A2->CCTL[0] & TIMER_A_CCTLN_CCIFG)
    {
        TIMER_A2->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  //clear interrupt flag
        enableConversion();

        //get current values of the string potentiometers
        while((ADC14->IFGR0 & ADC14_IFGR0_IFG1)==0) {};
        uint16_t string1 = ADC14->MEM[1];
        uint16_t string2 = ADC14->MEM[2]; //get string values
        uint16_t string3 = ADC14->MEM[3];

        //determine if any control buttons are pressed
        if(playSwitchPressed()) {

        }
        if(stopSwitchPressed()) {

        }
        if(saveSwitchPressed()) {

        }

        //run main function to update LCD
        mainLCD();

        //printf("\n\r value: %d", POTvalue);
    }
}
