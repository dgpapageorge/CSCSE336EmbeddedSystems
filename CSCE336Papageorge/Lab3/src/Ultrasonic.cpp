#include <Arduino.h>
#include <Servo.h>

#define SERVO_PIN 6
#define TRIGGER_PIN 7
#define ECHO_PIN 8
#define LEFT_LED_PIN 13
#define RIGHT_LED_PIN 2

Servo myservo;
int pos = 0;

// Ultrasonic sensor variables
volatile uint16_t pulseStartTime = 0;
volatile uint16_t pulseEndTime = 0;
volatile uint16_t pulseWidth = 0;

// LED variables
bool isLeftWallDetected = false;
bool isRightWallDetected = false;

void setup() {

  Serial.begin(9600);
  myservo.attach(6);

  // Configure Timer 1 for input capture with prescaler 8 and TOP value 0xFFFF
  TCCR1A = 0;
  TCCR1B = (1 << ICES1) | (1 << CS11) | (1 << ICNC1);
  TIMSK1 = (1 << ICIE1);

  // Configure Servo PWM on Timer 2
  TCCR2A = (1 << WGM20) | (1 << WGM21) | (1 << COM2B1);
  TCCR2B = (1 << CS21);
  OCR2A = 249;  // set TOP value to 249 for 50Hz frequency
  OCR2B = 38;  // set initial duty cycle to 1.5ms for center position

  // Set Servo PWM pin as output
  DDRD |= (1 << SERVO_PIN);

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
  /*while (pulseWidth == 0) {
    Serial.println(pulseWidth);
  }*/

  uint16_t distance = pulseWidth / 58;

  // Servo PWM pulse width based on the distance
  uint8_t servoPulseWidth = map(distance, 0, 12, 20, 50);
  OCR2B = servoPulseWidth;

  // Which LEDs to light up based on the distance and servo position
  if (distance <= 12) {
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

  pulseWidth = 0;
}

void servoSweep(){
  // move servo from -90 degrees to +90 degrees
  for (pos = -90; pos <= 90; pos += 1) { 
    OCR2B = map(pos, -90, 90, 26, 77);  // set duty cycle based on servo position
    delay(15);
  }
  // move servo from +90 degrees to -90 degrees
  for (pos = 90; pos >= -90; pos -= 1) { 
    OCR2B = map(pos, -90, 90, 26, 77);  // set duty cycle based on servo position
    delay(15);
  }
}

void pulse() {
  static uint8_t state = 0;
  static uint16_t pulseWidth = 0;
  static uint16_t lastTime = 0;
  uint16_t time = ICR1;

  if (state == 0) {
    // Capture rising edge
    lastTime = time;
    pulseWidth = 0;
    state = 1;
  } else {
    // Capture falling edge
    pulseWidth = time - lastTime;
    pulseWidth = (pulseWidth > 255) ? 255 : pulseWidth; // Limit pulse width to 255
    pulseWidth = (pulseWidth < 50) ? 0 : pulseWidth; // Ignore noise
    pulseWidth = pulseWidth / 2; // Divide by 2 to convert to microseconds
    pulseWidth = (pulseWidth > 255) ? 255 : pulseWidth; // Limit pulse width to 255
    wallLEDs();
    state = 0;
  }
}

void loop() {
  //servoSweep();
  //wallLEDs();
  pulse();
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