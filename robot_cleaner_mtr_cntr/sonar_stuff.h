#ifndef SONAR_STUFF_H_
#define SONAR_STUFF_H_

#include <NewPing.h>

#include <millisDelay.h>

millisDelay check_sonar;

#define MAX_DISTANCE 400
//#define TRIGGER_PIN_F 23 //bottom 1
//#define ECHO_PIN_F 23
#define TRIGGER_PIN_TF 17 //bottom 1
#define ECHO_PIN_TF 17
#define TRIGGER_PIN_TSR 5 //bottom 1
#define ECHO_PIN_TSR 5
#define TRIGGER_PIN_TSL 16 //bottom 1
#define ECHO_PIN_TSL 16
#define TRIGGER_PIN_L 4 //bottom 1
#define ECHO_PIN_L 4
#define TRIGGER_PIN_R 18 //bottom 1
#define ECHO_PIN_R 18

float duration; // Stores HC-SR04 pulse duration value
float distance; // Stores calculated distance in cm
int iterations = 1;

const int NUM_SONARS=5;

float soundsp = 343;
float soundcm = soundsp/10000; //cm/ms

//NewPing sonarf(TRIGGER_PIN_F,ECHO_PIN_F,MAX_DISTANCE);
NewPing sonartf(TRIGGER_PIN_TF,ECHO_PIN_TF,MAX_DISTANCE);
NewPing sonartsr(TRIGGER_PIN_TSR,ECHO_PIN_TSR,MAX_DISTANCE);
NewPing sonartsl(TRIGGER_PIN_TSL,ECHO_PIN_TSL,MAX_DISTANCE);
NewPing sonarr(TRIGGER_PIN_R,ECHO_PIN_R,MAX_DISTANCE);
NewPing sonarl(TRIGGER_PIN_L,ECHO_PIN_L,MAX_DISTANCE);

NewPing sonars_all[NUM_SONARS] = {sonarl, sonartsl, sonartf, sonartsr, sonarr};

bool reading_data = false;

const unsigned long WAIT_TO_GET_NEXT_READING = 35; 
unsigned long sonarCm;
unsigned long sonarPm;
unsigned long cntr = 0;

int current_sonar = 0;

float distances_all[NUM_SONARS] = {0,0,0,0,0};

float get_dist(int i){
  reading_data = true;

  int iterations = 1;
  float duration = sonars_all[i].ping_median(iterations);

  reading_data = false;

  return (duration / 2) * soundcm;
}

void start_sonar(){
  check_sonar.start(WAIT_TO_GET_NEXT_READING);
}

void get_sonar_data(){
  if(check_sonar.justFinished()&& !reading_data) {
    check_sonar.repeat();
    cntr++;
    distances_all[current_sonar] = get_dist(current_sonar);
    current_sonar++;
    if(current_sonar>=NUM_SONARS) {
      current_sonar = 0;
    }
    if (cntr >= NUM_SONARS*2) {
      cntr = 0;
      for (int j=0; j<NUM_SONARS;j++){
//        Serial.print("Sonar number ");
//        Serial.print(j);
//        Serial.print(": distance = ");
//        Serial.println(distances_all[j]);
      }
    }
  }
}

#endif
