#ifndef GYRO_STUFF_H_
#define GYRO_STUFF_H_

#include <BMI160Gen.h>
#include <MadgwickAHRS.h>
#include <Wire.h>
#include <millisDelay.h>

Madgwick filter;
unsigned long microsPerReading, microsPrevious;
unsigned long prnt_time_r, prnt_time_p;
float accelScale, gyroScale;
const int i2c_addr = 0x69;
float drift, drift0;
int aix, aiy, aiz;
int gix, giy, giz;
float ax, ay, az;
float gx, gy, gz;
float roll, pitch, heading;
unsigned long microsNow;
unsigned long prnt_time;

//millisDelay gyro_delay;

float convertRawAcceleration(int aRaw) {
  // since we are using 2G range
  // -2g maps to a raw value of -32768
  // +2g maps to a raw value of 32767
  
  float a = (aRaw * 2.0) / 32768.0;
  return a;
}

float convertRawGyro(int gRaw) {
  // since we are using 250 degrees/seconds range
  // -250 maps to a raw value of -32768
  // +250 maps to a raw value of 32767
  
  float g = (gRaw * 250.0) / 32768.0;
  return g;
}

float get_gyro_drift(){
  double drift_;
  
  BMI160.readMotionSensor(aix, aiy, aiz, gix, giy, giz);

    // convert from raw data to gravity and degrees/second units
    ax = convertRawAcceleration(aix);
    ay = convertRawAcceleration(aiy);
    az = convertRawAcceleration(aiz);
    gx = convertRawGyro(gix);
    gy = convertRawGyro(giy);
    gz = convertRawGyro(giz);

    // update the filter, which computes orientation
    filter.updateIMU(gx, gy, gz, ax, ay, az);

    // print the heading, pitch and roll
    roll = filter.getRoll();
    pitch = filter.getPitch();
    heading = filter.getYaw();
    Serial.println(heading);
    delay(prnt_time_r);
  BMI160.readMotionSensor(aix, aiy, aiz, gix, giy, giz);

    // convert from raw data to gravity and degrees/second units
    ax = convertRawAcceleration(aix);
    ay = convertRawAcceleration(aiy);
    az = convertRawAcceleration(aiz);
    gx = convertRawGyro(gix);
    gy = convertRawGyro(giy);
    gz = convertRawGyro(giz);

    // update the filter, which computes orientation
    filter.updateIMU(gx, gy, gz, ax, ay, az);
    drift_ = filter.getYaw() - heading;
//    Serial.println(filter.getYaw());

    return drift_;
    
}

void start_gyro() {
//  Serial.begin(9600);
//  Serial.println("setting up");
  // start the IMU and filter
  BMI160.begin(BMI160GenClass::I2C_MODE, i2c_addr);
  //Serial.println(BMI160.begin());
//  Serial.println("Currie begun");
  BMI160.setGyroRate(25);
  BMI160.setAccelerometerRate(25);
  filter.begin(25);

  // Set the accelerometer range to 2G
  BMI160.setAccelerometerRange(2);
  // Set the gyroscope range to 250 degrees/second
  BMI160.setGyroRange(250);
  BMI160.autoCalibrateAccelerometerOffset(X_AXIS, 0);
  BMI160.autoCalibrateAccelerometerOffset(Y_AXIS, 0);
  BMI160.autoCalibrateAccelerometerOffset(Z_AXIS, 1);

  // initialize variables to pace updates to correct rate
  microsPerReading = 1000000 / 25;
  microsPrevious = micros();
  prnt_time_p = millis();
  prnt_time_r = 500;
  drift = get_gyro_drift();
  drift0= drift;
  
//  gyro_delay.start(prnt_time_r);
}



void get_gyro_data(){
  prnt_time = millis();

  //Serial.println("in loop");

  // check if it's time to read data and update the filter
  microsNow = micros();
  if (prnt_time - prnt_time_p >= prnt_time_r) {
//    gyro_delay.repeat();
    // read raw data from BMI160
    BMI160.readMotionSensor(aix, aiy, aiz, gix, giy, giz);

    // convert from raw data to gravity and degrees/second units
    ax = convertRawAcceleration(aix);
    ay = convertRawAcceleration(aiy);
    az = convertRawAcceleration(aiz);
    gx = convertRawGyro(gix);
    gy = convertRawGyro(giy);
    gz = convertRawGyro(giz);

    // update the filter, which computes orientation
    filter.updateIMU(gx, gy, gz, ax, ay, az);

    // print the heading, pitch and roll
    roll = filter.getRoll();
    pitch = filter.getPitch();
    heading = filter.getYaw()-drift;
    drift += drift0;
    
    Serial.print("Orientation: ");
    Serial.print(heading);
    Serial.print(" ");
    Serial.print(pitch);
    Serial.print(" ");
    Serial.println(roll);
//    Serial.print(" ");
//    Serial.println(drift);
//    Serial.print("Orientation: ");
//    Serial.print(gix);
//    Serial.print(" ");
//    Serial.print(giy);
//    Serial.print(" ");
//    Serial.println(giz);
    prnt_time_p = prnt_time;
  }
}

void loop2() {
  
  prnt_time = millis();

  //Serial.println("in loop");

  // check if it's time to read data and update the filter
  microsNow = micros();
  if (microsNow - microsPrevious >= microsPerReading) {

    // read raw data from BMI160
    BMI160.readMotionSensor(aix, aiy, aiz, gix, giy, giz);

    // convert from raw data to gravity and degrees/second units
    ax = convertRawAcceleration(aix);
    ay = convertRawAcceleration(aiy);
    az = convertRawAcceleration(aiz);
    gx = convertRawGyro(gix);
    gy = convertRawGyro(giy);
    gz = convertRawGyro(giz);

    // update the filter, which computes orientation
    filter.updateIMU(gx, gy, gz, ax, ay, az);

    // print the heading, pitch and roll
    roll = filter.getRoll();
    pitch = filter.getPitch();
    heading = filter.getYaw();
    if (prnt_time - prnt_time_p >= prnt_time_r){
    Serial.print("Orientation: ");
    Serial.print(heading);
    Serial.print(" ");
    Serial.print(pitch);
    Serial.print(" ");
    Serial.println(roll);
    prnt_time_p = prnt_time;
    }

    // increment previous time, so we keep proper pace
    microsPrevious = microsPrevious + microsPerReading;
  }
}


#endif
