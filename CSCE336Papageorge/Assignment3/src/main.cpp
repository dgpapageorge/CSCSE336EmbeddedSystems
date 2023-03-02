/*--------------------------------------------------------------------  
Name: Demetri Papageorge 
Date: 2/28/23  
Course: CSCE 336 Embedded Systems (Fall 2022)  
File: main.cpp 
HW/Lab: Homework 3  
Purp: morse code Leds
Doc: no one helped me 
Academic Integrity Statement: I certify that, while others may have  
assisted me in brain storming, debugging and validating this program,  
the program itself is my own work. I understand that submitting code  
which is the work of other individuals is a violation of the honor  
code. I also understand that if I knowingly give my original work to  
another individual is also a violation of the honor code.  
--------------------------------------------------------------------*/
/****************************************************************************
*
*   Copyright (c) 2011 Carrick Detweiler
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*
****************************************************************************/

#include <avr/io.h>
#include <Arduino.h>


/**
 * Define a dot as a zero and dash as a 1
 **/
#define DOT 0
#define DASH 1

/**
 * These macros convert dots and dashes into a binary representation.
 * The upper three bits indicate the number of dots and dashes.  The
 * other bits contain the dots (0) and dashes (1).  The LSB is the
 * first dot/dash to send and so forth. 
 **/
#define MORSE_IN1(a)           ((1<<5) | (a<<0))
#define MORSE_IN2(a,b)         ((2<<5) | (a<<0) | (b<<1))
#define MORSE_IN3(a,b,c)       ((3<<5) | (a<<0) | (b<<1) | (c<<2))
#define MORSE_IN4(a,b,c,d)     ((4<<5) | (a<<0) | (b<<1) | (c<<2) | (d<<3))
#define MORSE_IN5(a,b,c,d,e)   ((5<<5) | (a<<0) | (b<<1) | (c<<2) | (d<<3) | (e<<4))

#define DDRD (*((volatile uint8_t *) 0x2A)) 
#define PORTD (*((volatile uint8_t *) 0x2B))
#define PIND (*((volatile uint8_t *) 0x29))
#define BLUE 5
#define RED 4
#define GREEN 3

/**
 * This is a table of all of the morse code strings for a-z.  If you
 * are passed a letter c you can access it in here by using
 * morseTable[c-'a'].  The upper three bits indicate the number of dots
 * and dashes in the code for that character.  The LSB is the first
 * dot/dash, next bit is the second, etc.
 **/
uint8_t morseTable[26] =
  {MORSE_IN2(DOT,DASH), //A
   MORSE_IN4(DASH,DOT,DOT,DOT), //B
   MORSE_IN4(DASH,DOT,DASH,DOT), //C
   MORSE_IN3(DASH,DOT,DOT), //D
   MORSE_IN1(DOT), //E
   MORSE_IN4(DOT,DOT,DASH,DOT), //F
   MORSE_IN3(DASH,DASH,DOT), //G
   MORSE_IN4(DOT,DOT,DOT,DOT), //H
   MORSE_IN2(DOT,DOT), //I
   MORSE_IN4(DOT,DASH,DASH,DASH), //J
   MORSE_IN3(DASH,DOT,DASH), //K
   MORSE_IN4(DOT,DASH,DOT,DOT), //L
   MORSE_IN2(DASH,DASH), //M
   MORSE_IN2(DASH,DOT), //N
   MORSE_IN3(DASH,DASH,DASH), //O
   MORSE_IN4(DOT,DASH,DASH,DOT), //P
   MORSE_IN4(DASH,DASH,DOT,DASH), //Q
   MORSE_IN3(DOT,DASH,DOT), //R
   MORSE_IN3(DOT,DOT,DOT), //S
   MORSE_IN1(DASH), //T
   MORSE_IN3(DOT,DOT,DASH), //U
   MORSE_IN4(DOT,DOT,DOT,DASH), //V
   MORSE_IN3(DOT,DASH,DASH), //W
   MORSE_IN4(DASH,DOT,DOT,DASH), //X
   MORSE_IN4(DASH,DOT,DASH,DASH), //Y
   MORSE_IN4(DASH,DASH,DOT,DOT)}; //Z


/**
 * Sends a long blink on the specified led.
 **/
void morseBlinkLong(uint8_t led){
  PORTD |= (1 << led);
  delay(500);
  PORTD &= ~(1 << led);

}

/**
 * Sends a short blink (a dot) on the specified led.
 **/
void morseBlinkShort(uint8_t led){
  PORTD |= (1 << led);
  delay(250);
  PORTD &= ~(1 << led);
  

}

/**
 * Perform a long pause (without turning on the LEDs), which should be
 * about the length of a dash.
 **/
void morseLongPause(void){
  PORTD &= ~(1 << RED);
  PORTD &= ~(1 << BLUE);
  PORTD &= ~(1 << GREEN);
  delay(1000);

}

/**
 * Perform a short pause (without turning on the LEDs), which should
 * be about the length of a dot.
 **/
void morseShortPause(void){
  PORTD &= ~(1 << RED);
  PORTD &= ~(1 << BLUE);
  PORTD &= ~(1 << GREEN);
  delay(200);
  

}

/**
 * Sends the right blink sequence for character c on the specified
 * led.  Note that the valid range for c is 'a'-'z' or 'A'-'Z'.
 **/
void morseBlinkChar(uint8_t led, char c){
  uint8_t i;

  //First make sure it is a valid character
  if(!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))){
    //Not valid
    return;
  }

  //If it is upper case, convert to lower case
  if(c >= 'A' && c <= 'Z'){
    c = c - 'A';
    c = c + 'a';
  }

  //Get the number of symbols in the morse code for c
  uint8_t numSymbols = morseTable[c-'a'] >> 5;
  //Now send each character
  for(i = 0; i < numSymbols; i++){
    //Send the right length pulse depending on if the i'th bit is a 1 or 0
    if(((morseTable[c-'a']>>i) & 0x1) == DOT){ morseBlinkShort(led); }
    else{ morseBlinkLong(led); } //DASH
    //Do a short pause, needed between symbols, ok for between
    //characters (but not long enough for between characters)
    morseShortPause();
  }
}


/**
 * Sends the specified string (with len characters) as morse code
 * blinks over the specified LED.  Note that the valid range for c is
 * 'a'-'z' or 'A'-'Z'.
 **/
void morseBlinkString(uint8_t led, char *c, uint8_t len){

  //STUDENT CODE 
  //Send a string of characters to be blinked.  Note that between
  //characters there should be a morseLongPause().  You can call
  //morseBlinkChar() to send each character.
  for(int i = 0; i<len; i++)
  {
    morseBlinkChar(led,c[i]);
    morseLongPause();
  }
  
}

uint8_t index = 0;
//Saves the inputted char from user and puts it into a char* at different indexes.
//Returns the starting char* when pressing enter.
char* morseInput(){
  char *word = new char[100];
  while (true) {
    if (Serial.available()>0){
      char letter = Serial.read();
      if (letter != '\n'){
        word[index] = letter;
        index++;
        Serial.println(letter);
      }
      else{
        word[index] = '\0';
        Serial.println(word[0]);
        return word;
      }
    }
  }
}

int start = 0;
int end = 0;
int flag = 0;//so d,D print once
//records how long the button is pressed and determines
//if it should return the letter for a dot or dash.
//also tells if there is a long pause.
String morseButton(){
  flag = 0;
  while(true){
    //button pressed
    while (!(PINC & (1 << PC2))){
      if (flag == 0){
        start = millis();
        flag = 1;
      }
    }
    if (flag == 1){
      end = millis();
      //dot timing
      if (end-start < 1000 && end-start > 200){
        return "d";
      }
      //dash timing
      else if (end-start > 1000){
        return "D";
      }
    }
    //long pause
    if (millis()-end > 1000){
      end = millis();
      return "s";
    }
  }
}

//turns on the specified LED through a morse code sequence
//either r,g,b for RED,GREEN,BLUE LEDs
void morseRGB(){
  int flag2 = 0;
  String code = "";
  String letter = "";
  while(flag2 == 0){
    //gets dot or dash or pause
    letter = morseButton();
    if (letter != "s"){
      code+=letter;
      Serial.println(letter);
    }
    else{
      //check if it matches r,g,b
      Serial.println(code);
      if (code == "dDd"){
        PORTD |= (1 << RED);
        delay(1000);
        PORTD &= ~(1 << RED);
      }
      else if (code == "DDd"){
        PORTD |= (1 << GREEN);
        delay(1000);
        PORTD &= ~(1 << GREEN);
      }
      else if (code == "Dddd"){
        PORTD |= (1 << BLUE);
        delay(1000);
        PORTD &= ~(1 << BLUE);
      }
      flag2 = 1;
    }
  }
}

//5b Turns on and off all 3 different LED colors
void LEDCheck(){
  PORTD |= (1 << RED);   
  delay(500);
  PORTD |= (1 << GREEN); 
  delay(500);
  PORTD &= ~(1 << RED);  
  delay(500);
  PORTD |= (1 << BLUE);   
  delay(500);
  PORTD &= ~(1 << GREEN); 
  delay(500);
  PORTD |= (1 << RED);   
  delay(500);
  PORTD &= ~(1 << BLUE); 
  PORTD &= ~(1 << RED);  
  delay(1000);
}


//Main code
#include <Arduino.h>

volatile int8_t result_8;
volatile int16_t result_16;
volatile int32_t result_32;
volatile float result_f;

#define DDRC (*((volatile uint8_t *) 0x27)) 
#define PORTC (*((volatile uint8_t *) 0x28))
#define PINC (*((volatile uint8_t *) 0x26))
#define PC2 2

void setup(){
  Serial.begin(9600);
  DDRC &= ~(1 << PC2);
  PORTC |= (1 << PC2);
  
  //#4 arduino commands for button
  //pinMode(PC2, INPUT);
  //digitalRead(PC2);
  //digitalWrite(PC2, HIGH);

  //Morse code LEDs
  DDRD |= (1 << RED);
  DDRD |= (1 << BLUE);
  DDRD |= (1 << GREEN);
  PORTD &= ~(1 << RED);
  PORTD &= ~(1 << BLUE);
  PORTD &= ~(1 << GREEN);

  //Hi in morse code
  char* Hi = new char[2];
  Hi[0] = 'H';
  Hi[1] = 'i';
  morseBlinkString(GREEN,Hi,1);
  morseLongPause();
  Hi++;
  morseBlinkString(RED,Hi,1);
}

//#2 divides and subtracts different int bit size numbers
void integerSubDiv(){
  int8_t a,b;
  int16_t c,d;
  int32_t e,f;
  float g,h;

  a = (int8_t) (rand() & 0xFF);
  b = (int8_t) (rand() & 0xFF);
  c = (int16_t) (rand() & 0xFFFF);
  d = (int16_t) (rand() & 0xFFFF);
  e = (int32_t) (rand() & 0xFFFFFFFF);
  f = (int32_t) (rand() & 0xFFFFFFFF);

  asm volatile("nop");
  result_8 = a-b;
  asm volatile("nop");
  result_16 = c-d;
  asm volatile("nop");
  result_32 = e-f;
  asm volatile("nop");
  result_8 = a/b;
  asm volatile("nop");
  result_16 = c/d;
  asm volatile("nop");
  result_32 = e/f;
  asm volatile("nop");

  g = (float) (rand() & 0xFF);
  h = (float) (rand() & 0xFF);

  asm volatile("nop");
  result_f = g-h;
  asm volatile("nop");
  result_f = g/h;
  asm volatile("nop");
}

//informs user of when the button is pressed
void pullUpButton(){
  if (!(PINC & (1 << PC2))){
    Serial.println("Pressed");
  }
  else{
    Serial.println("");
  }
}

//all functions ran for all parts
void loop(){
  char* word;
  //word = morseInput();
  //Serial.println(morseButton());
  morseRGB();
  //LEDCheck();
  //pullUpButton();
  //integerSubDiv();
}

/*
2a)
a-b: 
mov = 1
sub = 1
lds = 2
lds = 2
sts = 2
total 8 cycles

c-d:
movw = 1
sub = 1
sbc = 1
sts = 2
sts = 2
lds = 2
lds = 2
total - 11 cycles

e-f:
movw = 1
movw = 1
sub = 1
sbc = 1
sbc = 1
sbc = 1
sts = 2
sts = 2
sts = 2
sts = 2
lds = 2
lds = 2
total = 18 cycles

2b)
a-b:
8/16*10^6 = 5*10^-7s

c-d:
11/16*10^6 = 6.875*10^-7s

e-f:
18/16*10^6 = 1.125*10^-6s

2c)
a/b:
mov = 1
add = 1
sbc = 1
mov = 1
add = 1
sbc = 1
call = 4
bst = 1
mov = 1
brtc = .5
com = 1
rcall = 3
com = 1
neg = 1
sbci = 1
ret = 4
call = 4
sub = 1
sub = 1
ldi = 2
rjmp = 2
sbrc = 1
rcall = 3
com = 1
neg = 1
sbci = 1
brtc = .5
ret = 4
sts = 2
lds = 2
lds = 2
total = 51 cycles = 3.1875*10^-6s

c/d:
movw = 1
movw = 1
call = 4
bst = 1
mov = 1
brtc = .5
com = 1
rcall = 3
com = 1
neg = 1
sbci = 1
ret = 4
call = 4
sub = 1
sub = 1
ldi = 2
rjmp = 2
sbrc = 1
rcall = 3
com = 1
neg = 1
sbci = 1
brtc = .5
ret = 4
sts = 2
sts = 2
lds = 2
lds = 2
total = 49 cycles = 3.0625*10^-6s

e/f:
movw = 1
movw = 1
movw = 1
movw = 1
call = 4
mov = 1
bst = 1
brtc = .5
com = 1
call = 4
sbrc = 1
rcall = 3
com = 1
com = 1
com = 1
neg = 1
sbci = 1
sbci = 1
sbci = 1
call = 4
ldi = 2
mov = 1
sub = 1
sub = 1
movw = 1
rjmp = 2
sbrc = 1
rcall = 3
com = 1
com = 1
com = 1
neg = 1
sbci = 1
sbci = 1
sbci = 1
brtc = .5
jmp = 3
sts = 2
sts = 2
lds = 2
lds = 2
total = 61 cycles = 3.8125*10^-6s

2d)
g-h:
movw = 1
movw = 1
movw = 1
movw = 1
call = 4
subi = 1
sts = 2
sts = 2
sts = 2
sts = 2
lds = 2
lds = 2
total = 21 cycles = 1.3125*10^-6s

g/h = ~200 cycles from estimating the instuctions = 1.25*10^-5s

3a)
Red:
2.8V/180 = 15.55mA

Green/Blue:
2V/100 = 20mA

3b)
current = 40mA so about 2 LED's

3c) picture

5a)
In the function it takes the inputed character and checks if it is a valid character A-Z,a-z.
Then it convert it to lower case if its uppercase already by subtracting the ascii value of 'A' then adding the value of 'a' so that its at the same letter but lowercase.
Then it gets the bits from the array for the dots and dashes and checks them, 1 being a dash and 0 being a dot.
Then it runs the correct blink method for each dot and dash with a short pause in between dot/dash and long pause between chars
It gets the inputted characters from the morseInput method which records the user inputed characters and then calls the morseBlinkString method.

5b)
I connected these LEDs to pins 3,4,5 on PORTD made them an output by setting the DDRD bits 3,4,5 to 1.
I turned on and off the LEDs by changing the PORTD bits 3,4,5 on for 1 and off for 0.


*/
