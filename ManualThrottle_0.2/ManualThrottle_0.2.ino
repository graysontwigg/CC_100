/*
 * This program is used as a simple hand throttle for a 1984 VW Jetta GLI as a stepping point for full PID control
 * 
 * GRAYSON TWIGG 
 * 08/08/2016
 * 
 * The control scheme is as follows:
 * A servo is controlled via PWM(1000us to 2000us pulsewidth with 20ms period) to operate the throttle.
 * The user flips a switch to turn on the system(TPOTIO). While this switch is on a potentiometer(TPOT)
 * is read whos voltage is mapped to the PWM signal so that 0V is 1000us and 3.3V is 2000us. If at any
 * point while the switch is one a limit switch attached to the brake(BRAKE) or clutch(CLUTCH) are pressed
 * the PWM signal should be reset to 1000us and cause the main switch to be turned off and back on to read
 * the potentiometer value. Additionally if a limit switch attached to the accelerator pedal(GAS) is pressed
 * the system should hold the current PWM value and ignore the potentiometer value until the switch is
 * released; this system is unnecessary for manual throttle but will be required for PID control to be
 * implemented later.
 */

#include "CurieTimerOne.h"

// Digital I/O Variables
const int TPOTIO = 2;
const int GAS = 3;
const int BRAKE = 4;
const int CLUTCH = 5;
const int RESUME = 10;


// Analog Variables
const int PWMpin = 9;
int PWM = 1000;
unsigned int TPOT = 0;

//-----------------------------------------------
// SETUP
//-----------------------------------------------
void setup()
{
  Serial.begin(9600); //set up Serial Communication for debugging
  delay(1000);  // delay for serial response
  
  pinMode(TPOTIO, INPUT);  // TPOTIO as input
  pinMode(GAS, INPUT);
  pinMode(BRAKE, INPUT);
  pinMode(CLUTCH, INPUT);
  pinMode(RESUME, INPUT);
  
  CurieTimerOne.pwmStart(PWMpin, 51, 10000); // PWM on Pin 9 with 0-1023 duty cycle and 10000 usec period
}

//-----------------------------------------------
// MAIN
//-----------------------------------------------
void loop()
{
  while (digitalRead(TPOTIO) == 0)
  {
    Serial.println("THROTTLE POT ON");
    Serial.println(PWM);
    TPOT = analogRead(A1);  // Read throttle pot value
    PWM = map(TPOT, 0, 1023, 51, 100); // Map's 0-1023 value from throttle pot to the 1000us to 2000us 90 degree throw of servo
    CurieTimerOne.pwmStart(PWMpin, PWM, 20000); // PWM on Pin 9 with 0-1023 duty cycle and 20000 usec (20ms) period
    if (digitalRead(BRAKE) == 1 || digitalRead(CLUTCH) == 1)
    {
      Serial.println("BRAKE OR CLUTCH DEPRESSED");
      Serial.println("PLEASE TOGGLE SWITCH OR PRESS RESUME TO CONTINUE");
      CurieTimerOne.pwmStart(9, 51, 20000); // Essentially turn off Servo
      while(digitalRead(TPOTIO) == 0 && digitalRead(RESUME) == 1); //wait/do nothing while the switch is on and the resume button has not been pressed
    }
  }
}
