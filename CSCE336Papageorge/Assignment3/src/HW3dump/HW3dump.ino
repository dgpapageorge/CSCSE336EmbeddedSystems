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
  //pinMode(PC2, INPUT);
  //digitalRead(PC2);
  //digitalWrite(PC2, HIGH);
}

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

void pullUpButton(){
  if (!(PINC & (1 << PC2))){
    Serial.println("Pressed");
  }
  else{
    Serial.println("");
  }
}


void loop(){
  pullUpButton();
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

*/
