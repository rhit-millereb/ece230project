/**
 * Main File for the ECE230 Final Project
 *
 * Program uses hardware specified in the final report
 *
 * This file contains:
 *  1) Main program to initialize the hardware
 *  2) Switch/Pot Port and Pin configurations
 *  3) 10Hz timer that runs to determine button press and what to do
 *
 * Ethan Miller and Taylor DiSalvo
 * 5/21/2023
 */


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
#include "potToDigConvert.h"
#include "music.h"
#include "sysTickDelays.h"

#define MAXSAVES 20

uint16_t currentSave;
uint16_t selectedSave;
uint16_t savedLengths[MAXNOTES][MAXSAVES];
uint16_t savedPeriods[MAXNOTES][MAXSAVES];

uint16_t mainNoteLengths[MAXNOTES];
uint16_t mainNotePeriods[MAXNOTES];
uint16_t playNote;
uint16_t globalNoteLength;

uint16_t string1Note;
char note1;
char octave1;
uint16_t string2Note;
char note2;
char octave2;
uint16_t string3Note;
char note3;
char octave3;

char lcdMessage[200];

char lcdSetting; // 1 = showing string values, 2 = showing saved songs, 3 = welcome message
                //4 = popup
uint16_t lcdTime;

#define  Frequency10Hz 3200  // 50ms*32/ms = 1600

//function used to send a message to the LCD to be displayed
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

//function to configure the IO switches for the instrument
void configureSwitches(void) {
    //configure control switches
    P1->SEL0 &= BIT5+BIT6+BIT7;
    P1->SEL1 &= BIT5+BIT6+BIT7;
    P1->OUT |= BIT5+BIT6+BIT7;
    P1->REN |= BIT5+BIT6+BIT7;

    //configure string switches, select button
    P3->SEL0 &= BIT5+BIT6+BIT7+BIT0+BIT3;
    P3->SEL1 &= BIT5+BIT6+BIT7+BIT0+BIT3;
    P3->OUT |= BIT5+BIT6+BIT7+BIT0+BIT3;
    P3->REN |= BIT5+BIT6+BIT7+BIT0+BIT3;


}

/**
 * The Following are all the functions used to detect a button press
 * Four functions for control buttons, three for the string buttons
 */
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

bool selectSwitchPressed(void)
{
    char switchStatus = (P3->IN >> 0) & 1;
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
    char switchStatus = (P3->IN >> 3) & 1;
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


/**
 * The following are functions used to determine the period of the note to play
 * given the ADC value that has been sent
 * The are three functions: one for each string
 */
uint16_t setStringOneNote(uint16_t adcValue) {
    if(adcValue < 1000) {
        note1 = 'G';
        octave1 = '3';
        return NOTEG3; //first tone
    } else if (adcValue < 2000) {
        note1 = 'A';
        octave1 = '3';
        return NOTEA3; //second tone
    } else if (adcValue < 3000) {
        note1 = 'B';
        octave1 = '3';
        return NOTEB3; //third tone
    } else if (adcValue < 4000) {
        note1 = 'C';
        octave1 = '4';
        return NOTEC4; //fourth tone
    } else {
        note1 = 'D';
        octave1 = '4';
        return NOTED4; //fifth tone
    }
}

uint16_t setStringTwoNote(uint16_t adcValue) {
    if(adcValue < 1000) {
        note2 = 'E';
        octave2 = '4';
        return NOTEE4; //first tone
    } else if (adcValue < 2000) {
        note2 = 'F';
        octave2 = '4';
        return NOTEF4; //second tone
    } else if (adcValue < 3000) {
        note2 = 'G';
        octave2 = '4';
        return NOTEG4; //third tone
    } else if (adcValue < 4000) {
        note2 = 'A';
        octave2 = '4';
        return NOTEA4; //fourth tone
    } else {
        note2 = 'B';
        octave2 = '4';
        return NOTEB4; //fifth tone
    }
}

uint16_t setStringThreeNote(uint16_t adcValue) {
    if(adcValue < 1000) {
        note3 = 'C';
        octave3 = '5';
        return NOTEC5; //first tone
    } else if (adcValue < 2000) {
        note3 = 'D';
        octave3 = '5';
        return NOTED5; //second tone
    } else if (adcValue < 3000) {
        note3 = 'E';
        octave3 = '5';
        return NOTEE5; //third tone
    } else if (adcValue < 4000) {
        note3 = 'F';
        octave3 = '5';
        return NOTEF5; //fourth tone
    } else {
        note3 = 'G';
        octave3 = '5';
        return NOTEG5; //fifth tone
    }
}



//Configures a 10Hz timer to function as the primary action of the program
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


//State machine function that determine what to display on the LCD
/**
 * 1 = Normal
 * 2 = Show Note Length
 * 3 = Show welcome message
 * 4 = present a popup
 */
void mainLCD() {
    //determine if there is a popup
    if(lcdSetting == '4') {
        lcdTime--;

        //determine if popup is expired and dismiss
        if(lcdTime == 0) {
            lcdSetting = '1';
        }
    }

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
        sprintf(lcdMessage, "1   2   3   %d/%c%c  %c%c  %c%c  %d", playNote, note1, octave1, note2, octave2, note3, octave3, MAXNOTES);
        sendLCDMessage(lcdMessage);
    } else if (lcdSetting == '2') {
        //unused
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
	//setTestNotes();


	ConfigureTimerA0CCROInterrupt();

	initConversion();

	configureSwitches();

	/*
	 * MAIN LOOP
	 */
	while(1) {

	}

}

//function to clear the current song
void clearCurrentSong(void) {
    int i = 0;
    for (i = 0; i < MAXNOTES; i++)
    {
        mainNotePeriods[i] = 0;
        mainNoteLengths[i] = 0;
        playNote = 0;
    }
}


//function that runs when the save menu is played
//freezes the updating of the LCD and presents a series
//of menues for the user to interact with
void saveMenu(void) {
    //stroke already set for menu
    sprintf(lcdMessage, "Save Song? /Play=Y Stop=N");
    sendLCDMessage(lcdMessage);
    bool saveSong = false;
    while(true) {
        //hold and wait for input
        if(playSwitchPressed()) {
            saveSong = true;
            break;
        } else if (stopSwitchPressed()) {
            while(stopSwitchPressed()) {}//wait for release
            break;
        }
    }

    //save the song to the song array
    if(saveSong) {
        if(currentSave + 1 == MAXSAVES) {
            sprintf(lcdMessage, "All Saves Used");
            lcdTime = 50;
            lcdSetting = '4';
            sendLCDMessage(lcdMessage);
            return;
        } else if (playNote == 0) {
            sprintf(lcdMessage, "No Music to Save");
            lcdTime = 50;
            lcdSetting = '4';
            sendLCDMessage(lcdMessage);
            return;
        }

        //set the song lengths and notes to the current value
        int i=0;
        for(i=0; i<MAXNOTES; i++) {
            savedLengths[currentSave][i] = mainNoteLengths[i];
            savedPeriods[currentSave][i] = mainNotePeriods[i];
        }

        sprintf(lcdMessage, "Song Saved/Saved to Song%d", currentSave);
        lcdTime = 50;
        lcdSetting = '4';
        sendLCDMessage(lcdMessage);

        //increase current save
        currentSave++;
        return;
    }

    sprintf(lcdMessage, "View Saves? /Play=Y Stop=N");
    sendLCDMessage(lcdMessage);
    bool viewSaves = false;
    //ask to view saves
    while(true) {
        //wait for input
        if(playSwitchPressed()) {
            viewSaves = true;
            while(playSwitchPressed()) {} //wait for release
            break;
        } else if (stopSwitchPressed()) {
            return;
        }
    }

    sprintf(lcdMessage, "Song %d /Play Stop=Exit", selectedSave);
    sendLCDMessage(lcdMessage);
    //enter infinite loop to display current song
    while(viewSaves) {
        //determine button presses
        if(playSwitchPressed()) {
            while(playSwitchPressed()) {} //wait for release

            //send the speaker the current song
            setMusic(savedPeriods[selectedSave], savedLengths[selectedSave], MAXNOTES);
            play();

            lcdTime = 25;
            lcdSetting = '4';
            sprintf(lcdMessage, "Playing Song...");
            sendLCDMessage(lcdMessage);
            viewSaves = false;
        } else if (stopSwitchPressed()) {
            viewSaves = false;
        }

        //determine to move to next song
        if(selectSwitchPressed()) {
            while(selectSwitchPressed()) {} //wait for release
            selectedSave++;
            if(selectedSave>MAXSAVES-1) selectedSave = 0;//bound the value of the saved song
            sprintf(lcdMessage, "Song %d /Play Stop=Exit", selectedSave);
            sendLCDMessage(lcdMessage);
        }
    }

    return;
}

//function to determine what length of the note
uint16_t determineNoteLength(uint16_t time, uint16_t current) {
    if(time<EIGHTH_NOTE) {
        return EIGHTH_NOTE;
    }
    if(time == EIGHTH_NOTE) {
        sprintf(lcdMessage, "Eighth Note");
        sendLCDMessage(lcdMessage);
        return EIGHTH_NOTE;
    } else if(time == QUARTER_NOTE) {
        sprintf(lcdMessage, "Quarter Note");
        sendLCDMessage(lcdMessage);
        return QUARTER_NOTE;
    } else if (time == HALF_NOTE) {
        sprintf(lcdMessage, "Half Note");
        sendLCDMessage(lcdMessage);
        return HALF_NOTE;
    } else if (time == WHOLE_NOTE) {
        sprintf(lcdMessage, "Whole Note");
        sendLCDMessage(lcdMessage);
        return WHOLE_NOTE;
    }
    return current;
}

//interrupt of the 10Hz timer
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
        if(playSwitchPressed() && (playNote > 0)) {
            while(playSwitchPressed()) {} //wait for release

            setMusic(mainNotePeriods, mainNoteLengths, playNote);
            play();
        }
        if(stopSwitchPressed()) {
            stop();
        }
        if(saveSwitchPressed()) {
            //wait for switch to be released
            bool selectButton = false;
            while(saveSwitchPressed()) {
                if(selectSwitchPressed()) {
                    selectButton = true;
                    break;
                }
            }

            //if the section button was pressed, clear song
            if(selectButton) {
                clearCurrentSong();
                lcdTime = 25;
                lcdSetting = '4';
                sprintf(lcdMessage, "SONG CLEARED");
                sendLCDMessage(lcdMessage);
            } else {
                //otherwise ask to save the song or enter saves
                saveMenu();
            }
        }

        //run main function to update LCD
        mainLCD();

        //using the ADC values determine the notes on each string
        string1Note = setStringOneNote(string1);
        string2Note = setStringTwoNote(string2);
        string3Note = setStringThreeNote(string3);

        uint16_t holdLength = 0;
        uint16_t holdTimer = 0;
        uint16_t durationToPlay = 0;
        //determine if the string buttons are pressed
        if (stringOnePressed())
        {
            //wait fur button release
            while(stringOnePressed()) {
                holdTimer++;
                if(holdTimer%4==0) holdLength++;
                durationToPlay = determineNoteLength(holdLength, durationToPlay);
                if(holdLength >= 0xFFFF) {
                    //note at full, wait for release
                    while(stringOnePressed()) {}
                    break;
                }
            }


            mainNoteLengths[playNote] = durationToPlay;
            mainNotePeriods[playNote] = string1Note;
            playNote++;
        }
        if (stringTwoPressed())
        {
            //wait fur button release
            while (stringTwoPressed()) {
                holdTimer++;
                if (holdTimer % 4 == 0) holdLength++;
                durationToPlay = determineNoteLength(holdLength, durationToPlay);
                if(holdLength >= 0xFFFF) {
                    //note at full, wait for release
                    while(stringTwoPressed()) {}
                    break;
                }
            }
            mainNoteLengths[playNote] = durationToPlay;
            mainNotePeriods[playNote] = string2Note;
            playNote++;
        }
        if (stringThreePressed())
        {
            //wait fur button release
            while (stringThreePressed())
            {
                holdTimer++;
                if (holdTimer % 4 == 0) holdLength++;
                durationToPlay = determineNoteLength(holdLength, durationToPlay);
                if(holdLength >= 0xFFFF) {
                    //note at full, wait for release
                    while(stringThreePressed()) {}
                    break;
                }
            }
            mainNoteLengths[playNote] = durationToPlay;
            mainNotePeriods[playNote] = string3Note;
            playNote++;
        }
    }
}
