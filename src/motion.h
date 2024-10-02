#ifndef MOTION_H
#define MOTION_H

#include <MPU6050_6Axis_MotionApps612.h>
#include <I2Cdev.h>

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

#define CALIB_X 0.21
#define CALIB_Y 0.0
#define CALIB_Z 0.52

#define INTERRUPT_PIN 14

class Motion
{
private:
    /* data */
    // MPU control/status vars
    bool dmpReady = false;  // set true if DMP init was successful
    uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
    uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
    uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
    uint16_t fifoCount;     // count of all bytes currently in FIFO
    uint8_t fifoBuffer[64]; // FIFO storage buffer

    // orientation/motion vars
    Quaternion q;           // [w, x, y, z]         quaternion container
    Quaternion rotation_q;
    // VectorInt16 aa;         // [x, y, z]            accel sensor measurements
    // VectorInt16 gy;         // [x, y, z]            gyro sensor measurements
    // VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
    // VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
    VectorFloat gravity;    // [x, y, z]            gravity vector
    float euler[3];         // [psi, theta, phi]    Euler angle container
    float ypr[3];


    MPU6050 mpu;

public:
    Motion();
    ~Motion();
    bool setup();
    void update_rotation();
    void reset_rotation();
    void get_rotation(float &rot_x, float &rot_y,float &rot_z);
    void print_quat();
};




#endif // MOTION_H