/*! \file */
/*!
 * servoDriverTemplate.h
 * ECE230-02 Spring 2022-2023
 * Date: April 11, 2023
 *
 * Description: Servo motor driver for MSP432P4111 Launchpad.
 *              Assumes SMCLK configured with 48MHz HFXT as source.
 *              Uses Timer_A2 and P5.6 (TA2.1)
 *
 **                 MSP432P4111
 *             -------------------
 *         /|\|                   |
 *          | |                   |
 *          --|RST                |
 *            |      (TA2.1) P5.6 |----> Servo
 *            |                   |
 *            |              PJ.2 |------
 *       S1-->|P1.1               |     |
 *            |                   |    HFXT @ 48MHz
 *            |                   |     |
 *            |              PJ.3 |------
 *  Created on: 4/25/23
 *      Author: Ethan Miller
 */

#ifndef SERVODRIVER_H_
#define SERVODRIVER_H_

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

#include "msp.h"

//Servo control pin
#define ServoControl_Port P5
#define ServoControl_Pin BIT6

#define SMCLK 48000000 //Hz
#define TimerA0Prescaler 16 //Timer A prescaler
#define TimerA2Clock  SMCLK/TimerA0Prescaler
#define SERVO_FREQUENCY 50  //Hz
#define SERVO_MINIMUM   750 //us
#define SERVO_MAXIMUM   2250    //us

// DONE add tick count values for constants
#define SERVO_TMR_PERIOD                60000        // ticks for 50Hz signal
#define SERVO_MIN_ANGLE                 1250-500        // ticks for 0.375ms pulse // pulses changed based on
#define SERVO_MAX_ANGLE                 5750-500        // ticks for 2.45ms pulse  // actual servo
#define TEN_DEGREE_TICKS                250        // ticks 10 degree shift


/*!
 * \brief This function configures pins and timer for servo motor driver
 *
 * This function configures P5.6 as output pin for servo drive signal and
 *  initializes Timer_A2 CCR1 for PWM output
 *
 * Modified bit 6 of \b P2DIR register and \b P2SEL registers.
 * Modified \b TA2CTL, \b TA2CCTL1 and CCR registers.
 *
 * \return None
 */
extern void initServoMotor(void);


/*!
 * \brief This increments servo angle 10 degrees, with wrap-around
 *
 * This function increments servo angle by 10 degrees. If new angle exceeds max
 *  angle (+90), it wraps around to min angle (-90)
 *
 * Modified \b TA2CCR1 register.
 *
 * \return None
 */
extern void incrementTenDegree(void);


/*!
 * \brief This function sets angle of servo
 *
 * This function sets angle of servo to \a angle (between -90 to 90)
 *
 *  \param angle Angle in degrees to set servo (between -90 to 90)
 *
 * Modified \b TA2CCR1 register.
 *
 * \return None
 */
extern void setServoAngle(int8_t angle);


extern void setServoRatio(double ratio);

extern void resetServo();

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* SERVODRIVER_H_ */
