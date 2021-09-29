

// Import required libraries
#include "sonar_stuff.h"
#include "html_stuff.h"
#include "motor_driver_stuff.h"
#include "gyro_stuff.h"
#include "rotary_encoder_stuff.h"
#include <millisDelay.h>

unsigned long check_this_often = 150;
millisDelay check_delay;
millisDelay check_right;

float d1=0.0;
float d2=0.0;

const int output = 2; //can't remember what this is for
const float holes = 20.0;
const float circumference = 21.05; //cm. There was once a really fat guy in the British court and they used to call him sir cumference.
const float robot_width = 15;//cm
const float turning_circle = 48; //cm. Distance between wheels times pi. (pi*d).
const float pi = 3.14159265359;

typedef struct pos  {
  float x;
  float y;
  float theta;
} pos ;

typedef struct s_min  {
  float x;
  int y;
} s_min ;

pos robot_pos;
pos target_pos;
float theta;
float delta_theta;
float w1, w2;

bool test1, test2;

s_min sonar_min1;
s_min sonar_min2;

float alpha = 5.5;
float beta = 1.0;
int cnt_q = 0;

void turn_robot(float thet){
  counter1 = 0;
  counter2 = 0;
  if (thet > 0) robot_right();
  if (thet < 0) robot_left();
  d2 = ((float) counter2/holes)*circumference;
  
  while (d2 < turning_circle*abs(thet)/(2.0*pi)){
    d2 = ((float) counter2/holes)*circumference;
   //wait for robot to complete turn
   //blocking code here. Can deal with this with rtos.
    //probably don't need these two lines, but just give the loop something to do.
    if (thet > 0) robot_right();
    if (thet < 0) robot_left();
  } 
  counter1 = 0;
  counter2 = 0;
  robot_stop();
}

bool at_goal(pos p1, pos p2){
  return (abs(p1.y - p2.y)<4 && abs(p1.x - p2.x)<4);
}

double constrainAngle(double x){
    x = fmod(x + 180,360);
    if (x < 0)
        x += 360;
    return x - 180;
}

void update_robot_pos(){
  //note only using the middle three sonars. 45 degrees left, front, 45 degrees right
  if(true){
    sonar_min1 = {1000, -10}; //minimum sonar and index
    sonar_min2 = {1000, -10}; //second minimum sonar index
    d2 = ((float) counter2/holes)*circumference;
    d1 = ((float) counter1/holes)*circumference;
//    Serial.print("d2 ");
//    Serial.println(d2);
    robot_pos.x += d2*cos(theta);
    robot_pos.y += d2*sin(theta);
    counter1 = 0;
    counter2 = 0;
    target_pos.theta = atan2(target_pos.y - robot_pos.y, target_pos.x - robot_pos.x)-theta;  //theta is direction robot is facing.
//    Serial.print("target pos theta ");
//    Serial.println(target_pos.theta);
//    Serial.print("theta :");
//    Serial.println(theta);
    //check all inputs. find minimum 2. Calculate turn. Must be less than 10cm
    for(int i = 1; i<4; i++){
      //only checking centr from left to right
      if (distances_all[i] < sonar_min1.x &&(distances_all[i]>0)){
        sonar_min2.y = sonar_min1.y; //new minimum. copy current min to 2nd min.
        sonar_min2.x = sonar_min2.x;
        sonar_min1.x = distances_all[i];
        sonar_min1.y = i-2; //shift by 2 so that index 0 corresponds to front sonars.
                            // this works because of how I have my distances_all set up, you might need to change yours
                            //distances_all: 0 is side left, 1 is left 45, 2 is front, 3 is right 45 and 4 is right front.
                            //I only use 1,2 and 3.
      }
    }
    delta_theta = 0.0;
    if ( sonar_min1.x <= 12.0 && sonar_min1.x>0.0){
      //we need to change direction.
      if (abs(sonar_min1.y) == 1) { // plus or minus 1
        //side left or right, turn right or left.
        //find the weight for this, if 12.00 use 0.5 say.
        w1 = -sonar_min1.y*alpha/(sonar_min1.x-beta); //turn right is negative theta and sonar_min1.y = -1, if sonar is
                                                      // on left so we need to turn right.
        delta_theta = (1.0*target_pos.theta+w1*pi/4)/(1.0+abs(w1));
      }
      if (sonar_min1.y == 0) {
        //front, turn 90 degrees in direction of second highest.
        //find the weight for this, if 12.00 use 0.5 say. alpha = 5.5, so would be 5.5/(12-1) = 0.5.
        //if second min is defined turn direction awasy from side obstacel.
        w1 = (abs(sonar_min2.y==1)?-sonar_min2.y:1.0)*alpha/(sonar_min1.x-beta);
        
        delta_theta = (1.0*target_pos.theta+w1*pi/2)/(1.0+abs(w1)); //take weighted average of obstacle avoidance and target.
      }
    }
    else { 
      cnt_q++;
     if(cnt_q>8) {
      delta_theta =(constrainAngle(target_pos.theta));// min(target_pos.theta, pi/4);
      cnt_q = 0;
//      Serial.print(">10, delta theta; ");
//       Serial.println(delta_theta*57);
       }
    }
    if (abs(delta_theta) > pi/16){
      //only turn if angle is larger than pi/16. Otherwise ignore.
          turn_robot((constrainAngle(delta_theta)));
          theta += delta_theta;
//          Serial.print("theta now: ");
//          Serial.println(theta);
          theta = (constrainAngle(theta)); //bring theta into range (-pi,pi).
//          Serial.println(theta*57);
    }
    
  }
}


void setup(){
  myData.sliderValue = "000";
  myData.variable = "stop";
  
  // Serial port for debugging purposes
  Serial.begin(115200);
  robot_setup();
  sonarPm = millis();

  WiFi.mode(WIFI_AP_STA);
  
 

  // Connect to Wi-Fi
//  WiFi.begin(ssid, password);
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(1000);
//    Serial.println("Connecting to WiFi..");
//  }
//
//  // Print ESP Local IP Address
//  Serial.println(WiFi.localIP());
  robot_pos = {0.0,0.0, pi/2};
  target_pos = {0.0,300.0, pi/2};
  sonar_min1 = {1000, -1}; //minimum sonar and index
  sonar_min2 = {1000, -1};
  theta = pi/2;
//  start_server();  //controls to web. might replace with remote.
  start_rotary_encoder(); //set up rotary encoder. attach interupts
  start_sonar(); //sonar set up.
//  start_gyro();
  check_delay.start(check_this_often);
  test1 = false;
  test2 = false;
}
  
void loop() {
  robo = 0;
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


  if(check_delay.justFinished()){
    Serial.print("counter ");
    Serial.print(counter1);
    Serial.print(" ");
    Serial.println(counter2);
    if(at_goal(robot_pos, target_pos)){
      robot_stop();
      
//      robot_pos = {0.0,0.0, pi/2};
      //set new target if you want to keep the robot going and restart clock. Uncomment below two lines.
      //target_pos = {200.0,-200.0, -pi/2};
      //check_delay.repeat();
      Serial.println("stopped");
      
    }
    else{
      update_robot_pos();
      
      robot_fwd();
//      Serial.print("x ");
//      Serial.println(robot_pos.x);
//      Serial.println(robot_pos.y);
      check_delay.repeat();
    }
  }
  get_sonar_data();
  delay(1);
  yield();

}

void commented_out(){
//  if(check_delay.justFinished() && test1){
//    check_delay.repeat();
//    d1 = ((float) counter1/holes)*circumference;
//    d2 = ((float) counter2/holes)*circumference;
//    Serial.println(d2);
//    if (d2 > 50.0){
//      robot_stop();
//      counter1 = 0;
//      counter2 = 0;
//      test1 = false;
//      test2 = true;
//      check_right.start(check_this_often);
//    } else{  
//      robot_fwd();
////      check_delay.repeat();
//    }
//  }
//
//  if(check_right.justFinished()  && test2){
//    //right turn 90 degrees
//    check_right.repeat();
//    robot_right();
//    d1 = ((float) counter1/holes)*circumference;
//    d2 = ((float) counter2/holes)*circumference;
//    Serial.println(d2);
//    if (d2 >= turning_circle/4.0){
//      robot_stop();
//      counter1 = 0;
//      counter2 = 0;
//      test2 = false;
//    } else{  
//      robot_right();
////      check_delay.repeat();
//    }
//  }
//  if(check_delay.justFinished() && false){
//    if (d2 < 600)  check_delay.repeat();
//    robot_fwd();
//    d2 = ((float) counter2/holes)*circumference;
//    d1 = ((float) counter1/holes)*circumference;
//    Serial.print("d1 = ");
//    Serial.println(d1);
//    Serial.print("d2 = ");
//    Serial.println(d2);
//  }
}
