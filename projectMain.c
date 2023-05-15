


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
uint16_t playNote;

uint16_t string1Note;
char note1;
uint16_t string2Note;
char note2;
uint16_t string3Note;
char note3;

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

    //configure string switches
    P3->SEL0 &= BIT5+BIT6+BIT7;
    P3->SEL1 &= BIT5+BIT6+BIT7;
    P3->OUT |= BIT5+BIT6+BIT7;
    P3->REN |= BIT5+BIT6+BIT7;
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

bool stringOnePressed(void) {
    char switchStatus = (P3->IN >> 5) & 1;
    if (switchStatus == 1) {
       //button is not pressed
       return false;
    }
    else {
       //button is pressed
       return true;
    }
}
bool stringTwoPressed(void) {
    char switchStatus = (P3->IN >> 6) & 1;
    if (switchStatus == 1) {
       //button is not pressed
       return false;
    }
    else {
       //button is pressed
       return true;
    }
}
bool stringThreePressed(void) {
    char switchStatus = (P3->IN >> 7) & 1;
    if (switchStatus == 1) {
       //button is not pressed
       return false;
    }
    else {
       //button is pressed
       return true;
    }
}

uint16_t setStringOneNote(uint16_t adcValue) {
    if(adcValue < 1000) {
        return 0; //first tone
    } else if (adcValue < 2000) {
        return 1; //second tone
    } else if (adcValue < 3000) {
        return 2; //third tone
    } else {
        return 3; //fourth tone
    }
}

uint16_t setStringTwoNote(uint16_t adcValue) {
    if(adcValue < 1000) {
        return 0; //first tone
    } else if (adcValue < 2000) {
        return 1; //second tone
    } else if (adcValue < 3000) {
        return 2; //third tone
    } else {
        return 3; //fourth tone
    }
}

uint16_t setStringThreeNote(uint16_t adcValue) {
    if(adcValue < 1000) {
        return 0; //first tone
    } else if (adcValue < 2000) {
        return 1; //second tone
    } else if (adcValue < 3000) {
        return 2; //third tone
    } else {
        return 3; //fourth tone
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

void setTestNotes(void) {
    mainNoteLengths[0] = 35000;
    mainNotePeriods[0] = 50000;

    mainNoteLengths[1] = 5000;
    mainNotePeriods[1] = 40000;

    mainNoteLengths[2] = 5000;
    mainNotePeriods[2] = 30000;

    mainNoteLengths[3] = 5000;
    mainNoteLengths[4] = 5000;
    mainNoteLengths[5] = 5000;
    mainNoteLengths[6] = 5000;
    mainNoteLengths[7] = 5000;
    mainNoteLengths[8] = 5000;
    mainNoteLengths[9] = 5000;
    playNote = 3;
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
	playNote = 0;
	setTestNotes();
	note1 = 'A';
	note2 = 'B';
	note3 = 'C';
	string1Note = 0;
	string2Note = 1;
	string3Note = 2;

	ConfigureTimerA0CCROInterrupt();

	initConversion();

	configureSwitches();

	/*
	 * MAIN LOOP
	 */
	while(1) {

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
            setMusic(mainNotePeriods, mainNoteLengths, playNote);
            play();
        }
        if(stopSwitchPressed()) {
            stop();
        }
        if(saveSwitchPressed()) {

        }

        //run main function to update LCD
        mainLCD();

        string1Note = setStringOneNote(string1);
        string2Note = setStringTwoNote(string2);
        string3Note = setStringThreeNote(string3);

        //determine if the string buttons are pressed
        if (stringOnePressed())
        {

        }
        if (stringTwoPressed())
        {

        }
        if (stringThreePressed())
        {

        }

        printf("\n\r value1: %d", string1Note);
        printf(" value2: %d", string2Note);
        printf(" value3: %d", string3Note);
    }
}
