#ifndef ROTARY_ENCODER_STUFF_H_
#define ROTARY_ENCODER_STUFF_H_

#include <millisDelay.h>

const byte MOTOR1 = 23;  // Motor 1 Interrupt Pin - INT 0
const byte MOTOR2 = 19;  // Motor 2 Interrupt Pin - INT 1
unsigned long rotary_time=1000;
millisDelay rotary_delay;

volatile unsigned int counter1 = 0;
volatile unsigned int counter2 = 0;
 
// Float for number of slots in encoder disk
float diskslots = 20;  // Change to match value of encoder disk
 
// Interrupt Service Routines
 
// Motor 1 pulse count ISR
void IRAM_ATTR ISR_count1()  
{
  counter1++;  // increment Motor 1 counter value
} 
 
// Motor 2 pulse count ISR
void IRAM_ATTR ISR_count2()  
{
  counter2++;  // increment Motor 2 counter value
} 

void start_rotary_encoder(){
  attachInterrupt(digitalPinToInterrupt (MOTOR1), ISR_count1, RISING);  // Increase counter 1 when speed sensor pin goes High
  attachInterrupt(digitalPinToInterrupt (MOTOR2), ISR_count2, RISING);  // Increase counter 2 when speed sensor pin goes High
  rotary_delay.start(rotary_time);
}

void get_rotary_encoder_data(){

  if(rotary_delay.justFinished()){
    rotary_delay.repeat();
    Serial.println((counter1/20.0)*60.00);
    Serial.println((counter2/20.0)*60.00);
    counter1 = 0;
    counter2 = 0;
  }
  
}

#endif
