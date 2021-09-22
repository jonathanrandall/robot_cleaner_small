/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-web-server-slider-pwm/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#include <WiFi.h>
#include "html_stuff.h"
#include <NewPing.h>

#define MAX_DISTANCE 400
#define TRIGGER_PIN_F 23 //bottom 1
#define ECHO_PIN_F 23
#define TRIGGER_PIN_TF 5 //bottom 1
#define ECHO_PIN_TF 5
#define TRIGGER_PIN_TSR 18 //bottom 1
#define ECHO_PIN_TSR 18
#define TRIGGER_PIN_TSL 17 //bottom 1
#define ECHO_PIN_TSL 17
#define TRIGGER_PIN_L 19 //bottom 1
#define ECHO_PIN_L 19
#define TRIGGER_PIN_R 16 //bottom 1
#define ECHO_PIN_R 16

float duration; // Stores HC-SR04 pulse duration value
float distance; // Stores calculated distance in cm
int iterations = 5;

const int NUM_SONARS=6;

float soundsp = 343;
float soundcm = soundsp/10000; //cm/ms

NewPing sonarf(TRIGGER_PIN_F,ECHO_PIN_F,MAX_DISTANCE);
NewPing sonart1(TRIGGER_PIN_TF,ECHO_PIN_TF,MAX_DISTANCE);
NewPing sonart2(TRIGGER_PIN_TSR,ECHO_PIN_TSR,MAX_DISTANCE);
NewPing sonart3(TRIGGER_PIN_TSL,ECHO_PIN_TSL,MAX_DISTANCE);
NewPing sonarr(TRIGGER_PIN_R,ECHO_PIN_R,MAX_DISTANCE);
NewPing sonarl(TRIGGER_PIN_L,ECHO_PIN_L,MAX_DISTANCE);

NewPing sonars_all[NUM_SONARS] = {sonarf,sonart1, sonart2, sonart3, sonarr, sonarl};

bool reading_data = false;

const unsigned long WAIT_TO_GET_NEXT_READING = 40; 
unsigned long sonarCm;
unsigned long sonarPm;
unsigned long cntr = 0;

int current_sonar = 0;

float distances_all[NUM_SONARS] = {0,0,0,0,0,0};

float get_dist(int i){
  reading_data = true;
//  Serial.print("in get dust ");
//  Serial.println(i);
  int iterations = 1;
  float duration = sonars_all[i].ping_median(iterations);

  reading_data = false;

  return (duration / 2) * soundcm;
}



const int output = 2;


void setup(){
  myData.sliderValue = "000";
  myData.variable = "stop";
  
  // Serial port for debugging purposes
  Serial.begin(115200);
  robot_setup();
  sonarPm = millis();

  WiFi.mode(WIFI_AP_STA);
  
 

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());
  
//  register_peers();
  start_server();
}
  
void loop() {
  if(robo)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previous_time >= move_interval) {
      previous_time = currentMillis;
      robot_stop();
      char rsp[32];
      sprintf(rsp,"SPPED: %d",motor_speed);
//      Serial.println("Stopped");
      robo=0;
    }
  }
//  robot_fwd();
//  delay(11111);
//  robot_back();
//  delay(11111);
  delay(1);
  yield();

  if((sonarCm > sonarPm + WAIT_TO_GET_NEXT_READING)||true) {
    cntr++;
    distances_all[current_sonar] = get_dist(current_sonar);
    current_sonar++;
    if(current_sonar>9) {
      current_sonar = 0;
    }
    if (cntr > 10) {
      cntr = 0;
      for (int j=0; j<NUM_SONARS;j++){
//        Serial.print("Sonar number ");
//        Serial.print(j);
//        Serial.print(": distance = ");
//        Serial.println(distances_all[j]);
      }
    }
  }
  //vTaskDelete (NULL);
  // put your main code here, to run repeatedly:
  delay(1);
}
