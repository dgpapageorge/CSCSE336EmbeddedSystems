#include <Arduino.h>
#include <Servo.h>

#define SERVO_PIN 3
#define TRIGGER_PIN 7
#define ECHO_PIN 0
#define LEFT_LED_PIN 5
#define RIGHT_LED_PIN 2
#define LED 6

Servo myservo;
int pos = 0;
int counter = 0;

// Ultrasonic sensor variables
volatile uint16_t pulseStartTime = 0;
volatile uint16_t pulseEndTime = 0;
volatile uint16_t pulseWidth = 0;

// LED variables
bool isLeftWallDetected = false;
bool isRightWallDetected = false;

void setup() {

  Serial.begin(9600);

  // Configure Timer 1 for input capture with prescaler 8 and TOP value 0xFFFF
  TCCR1A = 0;
  TCCR1B = (1 << CS11); // prescale 8

  // Configure Servo PWM on Timer 2
  TCCR2A = (1 << WGM20) | (1 << WGM21) | (1 << COM2A1);
  TCCR2B = (1 << CS21)| (1 << CS20) | (1 << CS22); // prescale 1024
  OCR2A = 23;  // set initial duty cycle to 1.5ms for center position

  // Set Servo PWM pin as output
  DDRB |= (1 << SERVO_PIN);

  //set LED pin to output and turn off
  DDRD |= (1 << LED);
  PORTD &= ~(1 << LED);

  // Set Trigger pin as output and Echo pin as input
  DDRD |= (1 << TRIGGER_PIN);
  DDRB &= ~(1 << ECHO_PIN);

  // Set LED pins as output
  DDRB |= (1 << LEFT_LED_PIN) | (1 << RIGHT_LED_PIN);

  // Enable global interrupts
  sei();
}

void wallLEDs() {
  // Send a 10 microsecond trigger pulse to the Ultrasonic Sensor
  PORTD |= (1 << TRIGGER_PIN);
  delayMicroseconds(10);
  PORTD &= ~(1 << TRIGGER_PIN);
  // Wait for the input capture interrupt to capture the echo pulse width
  while (PINB&(1<<ECHO_PIN) == 0) {}
  TCNT1 = 0;
  ICR1 = 0;
  while (PINB&(1<<ECHO_PIN) == 1){
    //PORTD |= (1 << LED);
    
  }
  PORTD &= ~(1 << LED);
  counter = ICR1;
  counter *= 2;
  Serial.println(counter);
  delay(1000);  










  // Which LEDs to light up based on the distance and servo position
  /*if (distance <= 12) {
    if (servoPulseWidth >= 35) {
      isLeftWallDetected = true;
    } else if (servoPulseWidth <= 25) {
      isRightWallDetected = true;
    } else {
      isLeftWallDetected = true;
      isRightWallDetected = true;
    }
  } else {
    isLeftWallDetected = false;
    isRightWallDetected = false;
  }
  //Light up the correct LEDs
  if (isLeftWallDetected && isRightWallDetected) {
    digitalWrite(LEFT_LED_PIN, HIGH);
    digitalWrite(RIGHT_LED_PIN, HIGH);
  } else if (isLeftWallDetected) {
    digitalWrite(LEFT_LED_PIN, HIGH);
    digitalWrite(RIGHT_LED_PIN, LOW);
  } else if (isRightWallDetected) {
    digitalWrite(RIGHT_LED_PIN, HIGH);
    digitalWrite(LEFT_LED_PIN, LOW);
  } else {
    digitalWrite(LEFT_LED_PIN, LOW);
    digitalWrite(RIGHT_LED_PIN, LOW);
  }

  pulseWidth = 0;*/
}

void servoSweep(){
  //23 counts = 1.5ms
  OCR2A = 23; // center
  delay(1000);
  OCR2A = 7; // left
  delay(1000);
  OCR2A = 37; // right
  delay(1000);
  OCR2A = 23; // center
  delay(1000);
}


void loop() {
  //servoSweep();
  wallLEDs();
}



/*
1.
The sensor waves travel at the speed of sound at 1125 feet per second

2.
58.26 micro seconds

3.
range of 2-400cm and an accuracy of 3mm

4.
minimum recommended measurement cycle is 60ms
the working frequency is 40khz, with a lower frequency it increases the accuracy but shortens the range

*/