/*  

    Impulse Generator v1.03 (5th May 2014)

*/

#include <Wire.h>
#include <Bounce2.h>

#define DS3231_I2C_ADDRESS 104     // DS3231 is I2C address 104

Bounce retardDebouncer = Bounce();
Bounce advanceDebouncer = Bounce(); 
Bounce retardHourDebouncer = Bounce(); 
Bounce advanceHourDebouncer = Bounce(); 

// CONFIGURATION ///////////////////////////////////////////////////
//
#define DEBUG
byte maxPulse = 30;          // Should be 30 unless debugging.
byte pulseLength = 150;      // Pulse duration (ms).
byte brightness = 255;       // Brightness level of LEDs.
//
////////////////////////////////////////////////////////////////////

// Interrupt
byte clockInt = 0;           // Digital 2 is INT0

// Inputs
byte retardButton = 8;
byte advanceButton = 13;
byte retardHourButton = 12;
byte advanceHourButton = 9;

// Outputs
byte onePulse = 7;
byte sixPulse = 4;
byte thirtyPulse = 3;
byte pendulumLed = 10;
byte threeLed = 6;
byte twoLed = 5;
byte oneLed = 11;

// Variables
byte seconds = 0;
unsigned long pulsedOneAt = 0;
unsigned long pulsedSixAt = 0;
unsigned long pulsedThirtyAt = 0;
byte buttonLockBy = 0;
int adjustment = 0;

unsigned long retardButtonPressTimeStamp;
unsigned long advanceButtonPressTimeStamp;
unsigned long retardHourButtonPressTimeStamp;
unsigned long advanceHourButtonPressTimeStamp;

boolean onePulseState, sixPulseState, thirtyPulseState, disableCounterLeds, retardButtonState, advanceButtonState, retardHourButtonState, advanceHourButtonState;

boolean pulsesEnabled = 1;
boolean pendulumLedState = 1;


// Setup ////////////////////////////////////////////////////////////
void setup()
{
  
  // Set control register to output square wave from DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x0E);
  Wire.write(B00000000);    // 1Hz = B00000000, 1024Hz = B00001000, 4096Hz = B00010000, 8192Hz = B00011000
  Wire.endTransmission();
  
  attachInterrupt(clockInt, clockControl, RISING);    // Interrupt occurs on rising SQW from DS3231
  
  // Inputs
  pinMode(retardButton,INPUT);
  pinMode(advanceButton,INPUT);
  pinMode(retardHourButton,INPUT);
  pinMode(advanceHourButton,INPUT);
  digitalWrite(retardButton,LOW);
  digitalWrite(advanceButton,LOW);
  digitalWrite(retardHourButton,LOW);
  digitalWrite(advanceHourButton,LOW);
  retardDebouncer.attach(retardButton);
  retardDebouncer.interval(5);
  advanceDebouncer.attach(advanceButton);
  advanceDebouncer.interval(5);
  retardHourDebouncer.attach(retardHourButton);
  retardHourDebouncer.interval(5);
  advanceHourDebouncer.attach(advanceHourButton);
  advanceHourDebouncer.interval(5);

  // Outputs
  pinMode(onePulse,OUTPUT);
  pinMode(sixPulse,OUTPUT);
  pinMode(thirtyPulse,OUTPUT);
  pinMode(pendulumLed,OUTPUT);
  pinMode(threeLed,OUTPUT);
  pinMode(twoLed,OUTPUT);
  pinMode(oneLed,OUTPUT);
  
  // Serial Debug
  #ifdef DEBUG
    Serial.begin(1200);
    #define DEBUG_PRINT(x) Serial.print(x)
    DEBUG_PRINT("\r\nR\r\n");
  #else
    #define DEBUG_PRINT(x)
  #endif

}


// Pulse ////////////////////////////////////////////////////////////
void pulse(byte pulseType) {

  if (pulsesEnabled) {

      if (pulseType == 1) {
        onePulseState = 1;
        digitalWrite(onePulse,HIGH);
        pulsedOneAt = millis();
      }

      if (pulseType == 6) {
        sixPulseState = 1;
        digitalWrite(sixPulse,HIGH);
        pulsedSixAt = millis();
      }

      if (pulseType == 30) {
        thirtyPulseState = 1;
        digitalWrite(thirtyPulse,HIGH);
        pulsedThirtyAt = millis();
      }

  }

}


// Pulse Control ////////////////////////////////////////////////////
void runPulses() {

  // One Second
   pulse(1);
   DEBUG_PRINT(seconds);     

   // Six Seconds
   if ((seconds % 6) == 0) {
     pulse(6);
     DEBUG_PRINT(" -6- ");  
   }

   // Thirty Seconds
   if (seconds == maxPulse) {
     pulse(30);
     DEBUG_PRINT("--30--\r\n");  
   }

   // RAM Check
   #ifdef DEBUG
     if ((seconds % 6) == 0 && seconds != 30) {
       Serial.print("(");
       Serial.print(freeRam());
       Serial.print(") ");
     }
   #endif

}


// Interrupt on INT0 ////////////////////////////////////////////////
void clockControl()
{
 
  // Everything in here happens every second, unless the DS3231 has gone wild. :)
  seconds ++;

  if (adjustment == 0) {
    
    runPulses();

  } else {
    
    // Retard
    // Hold all pulses until cancelled.
    //// This is handled by changing to pulsesEnabled.
    
    // Advance
    // Trigger thirtyPulse every second until cancelled.
    if (adjustment == 9999) {
      pulse(30);
      seconds = 0;
    }
    
    // Retard Hour
    // Hold all pulses until adjustment has elapsed.
    if (adjustment < 0) {

      if (adjustment == -1) {

        disableCounterLeds = 0;
        analogWrite(twoLed,0);
        retardHourButtonState = 0;
        buttonLockBy = 0;
        DEBUG_PRINT("\r\n");

      }

      adjustment++;

    }
    
    // Advance Hour
    // Trigger thirtyPulse on odd seconds in addition to normal output.
    if (adjustment > 0 && adjustment != 9999) {

      runPulses();

      if ((seconds % 2) != 0) {
        pulse(30);
      }

      if (adjustment == 1) {

        disableCounterLeds = 0;
        analogWrite(oneLed,0);
        analogWrite(twoLed,0);
        analogWrite(threeLed,0);
        advanceHourButtonState = 0;
        buttonLockBy = 0;
        DEBUG_PRINT("\r\n");

      }

      adjustment--;

    }
    
  }
  

  if (seconds == maxPulse) seconds = 0;    // Counter Reset
  

  // Pendulum LEDs
  if (pendulumLedState) {
    analogWrite(pendulumLed,brightness);
    pendulumLedState = 0;
  } else {
    analogWrite(pendulumLed,0);
    pendulumLedState = 1;
  }
 
  // Countdown LEDs
  if (!disableCounterLeds) {
 
    if (seconds == 27) {
      analogWrite(threeLed,brightness);
    } else {
      analogWrite(threeLed,0);
    }
    
    if (seconds == 28) {
      analogWrite(twoLed,brightness);
    } else {
      analogWrite(twoLed,0);
    }
    
    if (seconds == 29) {
      analogWrite(oneLed,brightness);
    } else {
      analogWrite(oneLed,0);
    }
  
  }

}


// Loop /////////////////////////////////////////////////////////////
void loop()
{
  

  //// End Pulses
  unsigned long runtime = millis();
  
  if (onePulseState == 1 && runtime - pulsedOneAt > pulseLength) {
    digitalWrite(onePulse,0);
    onePulseState = 0;
  }
  
  if (sixPulseState == 1 && runtime - pulsedSixAt > pulseLength) {
    digitalWrite(sixPulse,0);
    sixPulseState = 0;
  }
  
  if (thirtyPulseState == 1 && runtime - pulsedThirtyAt > pulseLength) {
    digitalWrite(thirtyPulse,0);
    thirtyPulseState = 0;
  }
  

  //// Handle Buttons
  boolean retardChanged = retardDebouncer.update();
  boolean advanceChanged = advanceDebouncer.update();
  boolean retardHourChanged = retardHourDebouncer.update();
  boolean advanceHourChanged = advanceHourDebouncer.update();
  boolean someButtonInAction = 0;

  // The retard button halts all pulses.
  if (retardChanged) {
    
    int value = retardDebouncer.read();
  
    if (value == HIGH) {

      if (retardButtonState == 0 && buttonLockBy == 0) {

        pulsesEnabled = 0;
        disableCounterLeds = 1;
        analogWrite(oneLed,brightness);
        analogWrite(twoLed,0);
        analogWrite(threeLed,0);
        retardButtonState = 1;
        buttonLockBy = 1;

      } else if (retardButtonState == 1 && buttonLockBy != 0) {

        pulsesEnabled = 1;
        disableCounterLeds = 0;
        seconds = 0;
        analogWrite(oneLed,0);
        retardButtonState = 0;
        buttonLockBy = 0;
        DEBUG_PRINT("\r\n");

      }

    }

  }
  
  // The advance button forces all pulses every second.
  if (advanceChanged) {
  
    int value = advanceDebouncer.read();

    if (value == HIGH) {

      if (advanceButtonState == 0 && buttonLockBy == 0) {

        adjustment = 9999;
        disableCounterLeds = 1;
        analogWrite(oneLed,0);
        analogWrite(twoLed,0);
        analogWrite(threeLed,brightness);
        advanceButtonState = 1;
        buttonLockBy = 2;

      } else if (advanceButtonState == 1 && buttonLockBy !=0) {

        adjustment = 0;
        disableCounterLeds = 0;
        analogWrite(threeLed,0);
        advanceButtonState = 0;
        buttonLockBy = 0;
        DEBUG_PRINT("\r\n");

      }

    }
    
  }
  
  // The retard hour button halts all pulses for 3600 seconds.
  if (retardHourChanged) {
  
    int value = retardHourDebouncer.read();
  
    if (value == HIGH) {
      
      if (retardHourButtonState == 0 && buttonLockBy == 0) {
        
        adjustment = -3600;
        disableCounterLeds = 1;
        analogWrite(oneLed,0);
        analogWrite(twoLed,brightness);
        analogWrite(threeLed,0);
        retardHourButtonState = 1;
        buttonLockBy = 3;
    
      } else if (retardHourButtonState == 1 && buttonLockBy !=0) {
      
        adjustment = 0;
        disableCounterLeds = 0;
        analogWrite(twoLed,0);
        retardHourButtonState = 0;
        buttonLockBy = 0;
      
      }
    
    }
    
  }
  
  if (advanceHourChanged) {
    
    int value = advanceHourDebouncer.read();
  
    if (value == HIGH) {

      if (advanceHourButtonState == 0 && buttonLockBy == 0) {
        
        adjustment = 240;
        disableCounterLeds = 1;
        analogWrite(oneLed,brightness);
        analogWrite(twoLed,brightness);
        analogWrite(threeLed,brightness);
        advanceHourButtonState = 1;
        buttonLockBy = 4;
    
      } else if (advanceHourButtonState == 1 && buttonLockBy !=0) {
      
        adjustment = 0;
        disableCounterLeds = 0;
        analogWrite(oneLed,0);
        analogWrite(twoLed,0);
        analogWrite(threeLed,0);
        advanceHourButtonState = 0;
        buttonLockBy = 0;
      
      }
    
    }
    
  }
  
}


// RAM Check /////////////////////////////////////////////////////////////
#ifdef DEBUG
  int freeRam () {
    extern int __heap_start, *__brkval; 
    int v; 
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
  }
#endif
