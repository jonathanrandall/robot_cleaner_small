#ifndef MOTOR_DRIVE_STUFF_H_
#define MOTOR_DRIVE_STUFF_H_

int gamma1 = 0;
// Motor right
 
uint8_t enA = 12;
int in1 = 27;
int in2 = 26;
 
// Motor left
 
uint8_t enB = 13;
int in3 = 32;
int in4 = 33;

// Define Speed variables

int noStop = 0;

//Setting Motor PWM properties
double freq = 2000;
const int motorPWMChannnelA = 7;
const int motorPWMChannnelB = 2;
uint8_t lresolution = 8;
 
uint8_t  motor_speed   = 150;
volatile unsigned long previous_time = 0;
volatile unsigned long move_interval = 250;



void update_speed()
{ 
    ledcWrite(motorPWMChannnelA, motor_speed);

//    Serial.println("motor_speed");
    
    ledcWrite(motorPWMChannnelB, motor_speed+gamma1);

//    Serial.println(motor_speed);
    
}

int LEFT_M0 = in1;
int LEFT_M1 = in2;
int RIGHT_M0 = in3;
int RIGHT_M1 = in4;

enum state {fwd, rev, stp};
state actstate = stp;
uint8_t robo = 0;

void robot_stop()
{
  digitalWrite(in1,LOW);
  digitalWrite(in2,LOW);
  digitalWrite(in3,LOW);
  digitalWrite(in4,LOW);
}

void robot_fwd()
{
  digitalWrite(LEFT_M0,HIGH);
  digitalWrite(LEFT_M1,LOW);
  digitalWrite(RIGHT_M0,HIGH);
  digitalWrite(RIGHT_M1,LOW);
  move_interval=250;
  previous_time = millis();  
}
 
void robot_back()
{
  digitalWrite(LEFT_M0,LOW);
  digitalWrite(LEFT_M1,HIGH);
  digitalWrite(RIGHT_M0,LOW);
  digitalWrite(RIGHT_M1,HIGH);
  move_interval=250;
   previous_time = millis();  
}
 
void robot_right()
{
  digitalWrite(LEFT_M0,LOW);
  digitalWrite(LEFT_M1,HIGH);
  digitalWrite(RIGHT_M0,HIGH);
  digitalWrite(RIGHT_M1,LOW);
  move_interval=100;
   previous_time = millis();
}
 
void robot_left()
{
  digitalWrite(LEFT_M0,HIGH);
  digitalWrite(LEFT_M1,LOW);
  digitalWrite(RIGHT_M0,LOW);
  digitalWrite(RIGHT_M1,HIGH);
  move_interval=100;
   previous_time = millis();
}

void robot_setup(){

  // Pins for Motor Controller
    pinMode(in1,OUTPUT);
    pinMode(in2,OUTPUT);
    pinMode(in3,OUTPUT);
    pinMode(in4,OUTPUT);
    
    // Make sure we are stopped
    robot_stop();
 
    // Motor uses PWM Channel 8
    
    delay(100);
    
    Serial.println(ledcSetup(motorPWMChannnelA, freq, lresolution));
    Serial.println(ledcSetup(motorPWMChannnelB, freq, lresolution));
    
    
    
    ledcAttachPin(enA, motorPWMChannnelA);
    ledcAttachPin(enB, motorPWMChannnelB);

    ledcWrite(motorPWMChannnelA, 130);
    ledcWrite(motorPWMChannnelB, 130);
    
  
}



#endif
