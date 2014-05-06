/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */


int onePulse = 7;
int sixPulse = 4;
int thirtyPulse = 3;
int pendulumLed = 10;
int threeLed = 6;
int twoLed = 5;
int oneLed = 11;

int ourspeed = 75;
int ourpause = 100;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(onePulse, OUTPUT);
  pinMode(sixPulse, OUTPUT);
  pinMode(thirtyPulse, OUTPUT);  
  pinMode(pendulumLed, OUTPUT);
  pinMode(threeLed, OUTPUT);     
  pinMode(twoLed, OUTPUT);     
  pinMode(oneLed, OUTPUT);     
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(pendulumLed, HIGH);
  delay(ourspeed);
  digitalWrite(threeLed, HIGH);
  digitalWrite(onePulse, HIGH);
  delay(ourspeed);
  digitalWrite(twoLed, HIGH);
  digitalWrite(sixPulse, HIGH);
  delay(ourspeed);
  digitalWrite(oneLed, HIGH);
  digitalWrite(thirtyPulse, HIGH);
  delay(ourpause);
  digitalWrite(pendulumLed, LOW);
  delay(ourspeed);
  digitalWrite(threeLed, LOW);
  digitalWrite(onePulse, LOW);
  delay(ourspeed);
  digitalWrite(twoLed, LOW);
  digitalWrite(sixPulse, LOW);
  delay(ourspeed);
  digitalWrite(oneLed, LOW);
  digitalWrite(thirtyPulse, LOW);
  delay(ourspeed);
}
