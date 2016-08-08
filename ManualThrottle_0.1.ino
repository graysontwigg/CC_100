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
volatile byte TPOTIO = HIGH;
unsigned long DEBOUNCET = 0;
unsigned long LAST_DEBOUNCET = 0;

boolean GAS = 1;
//volatile byte GAS = HIGH;
unsigned long DEBOUNCEG = 0;
unsigned long LAST_DEBOUNCEG = 0;

//boolean BRAKE = 1;
volatile byte BRAKE = HIGH;
unsigned long DEBOUNCEB = 0;
unsigned long LAST_DEBOUNCEB = 0;

//boolean CLUTCH = 1;
volatile byte CLUTCH = HIGH;
unsigned long DEBOUNCEC = 0;
unsigned long LAST_DEBOUNCEC = 0;

// Analog Variables
int PWM = 1000;
unsigned int TPOT = 0;

//-----------------------------------------------
// SETUP
//-----------------------------------------------
void setup()
{
  Serial.begin(9600); //set up Serial Communication for debugging
  delay(1000);  // delay for serial response
  
  pinMode(2, INPUT);  // TPOTIO as input
  attachInterrupt(2, TPOTIO_I, CHANGE);
  pinMode(3, INPUT);  // GAS as input
  //attachInterrupt(3, GAS_I, CHANGE);
  pinMode(4, INPUT);  // BRAKE as input
  attachInterrupt(4, BRAKE_I, CHANGE);
  pinMode(5, INPUT);  // CLUTCH as input
  attachInterrupt(5, CLUTCH_I, CHANGE);
  
  CurieTimerOne.pwmStart(9, 51, 10000); // PWM on Pin 9 with 0-1023 duty cycle and 10000 usec period
}

//-----------------------------------------------
// MAIN
//-----------------------------------------------
void loop()
{
  while (!TPOTIO)
  {
    Serial.println("THROTTLE POT ON");
    Serial.print(TPOT);
    Serial.print("\t");
    Serial.println(PWM);
    TPOT = analogRead(A1);  // Read throttle pot value
    PWM = map(TPOT, 0, 1023, 51, 102); // Map's 0-1023 value from throttle pot to the 1000us to 2000us 90 degree throw of servo
    CurieTimerOne.pwmStart(9, PWM, 20000); // PWM on Pin 9 with 0-1023 duty cycle and 20000 usec (20ms) period

    /*
    if(digitalRead(4) == 0 || digitalRead(5) == 0)
    {
      CurieTimerOne.pwmStart(9, 51, 20000);
      TPOTIO = !TPOTIO;
    }
    */
    
    if (!BRAKE)
    {
      CurieTimerOne.pwmStart(9, 51, 20000); // Essentially turn off Servo
      TPOTIO = !TPOTIO; //Reset the throttle pot state to force it to be reset
      Serial.print("Brake: \t");
      Serial.print(BRAKE);
      Serial.print("Clutch: \t");
      Serial.print(CLUTCH);
    }
  }
}

//-----------------------------------------------
// TPOTIO
//-----------------------------------------------
void TPOTIO_I()
{
  DEBOUNCET = millis();
  if (DEBOUNCET - LAST_DEBOUNCET > 250) // Debounce of 250ms
  {
    TPOTIO = !TPOTIO;
    LAST_DEBOUNCET = DEBOUNCET; // Set debounce time
  }
}

//-----------------------------------------------
// GAS
//-----------------------------------------------
void GAS_I()
{
  DEBOUNCEG = millis();
  if (DEBOUNCEG - LAST_DEBOUNCEG > 10) // Debounce of 250ms
  {
   GAS = !GAS;
    LAST_DEBOUNCEG = DEBOUNCEG; // Set debounce time
  }
}

//-----------------------------------------------
// BRAKE
//-----------------------------------------------
void BRAKE_I()
{
  BRAKE = !BRAKE;
  DEBOUNCEB = millis();
  if (DEBOUNCEB - LAST_DEBOUNCEB > 10) // Debounce of 250ms
  {
    BRAKE = !BRAKE;
    LAST_DEBOUNCEB = DEBOUNCEB; // Set debounce time
  }
}

//-----------------------------------------------
// CLUTCH
//-----------------------------------------------
void CLUTCH_I()
{
  DEBOUNCEC = millis();
  if (DEBOUNCEC - LAST_DEBOUNCEC > 10) // Debounce of 250ms
  {
    CLUTCH = !CLUTCH;
    LAST_DEBOUNCEC = DEBOUNCEC; // Set debounce time
  }
}

