#include <motion.h>

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
}

Motion::Motion(){
}



Motion::~Motion(){
}

Quaternion quat_inverse(Quaternion q){
    float mag = q.getMagnitude();
    Quaternion inverse = q.getConjugate();
    float mag2 = mag * mag;
    inverse.w /= mag2;
    inverse.x /= mag2;
    inverse.y /= mag2;
    inverse.z /= mag2;
    return inverse;
}

Quaternion rotate(Quaternion q1, Quaternion q2){
    Quaternion q = q1.getProduct(q2);
    return q.getProduct(quat_inverse(q1));
}


bool Motion::setup()
{
    Wire.begin();
    Wire.setClock(400000);

    mpu.initialize();
    pinMode(INTERRUPT_PIN, INPUT);

    if (!mpu.testConnection()){
        Serial.println("MPU connection failed...");
        return false;
    }

    devStatus = mpu.dmpInitialize();
    // calculated by https://github.com/jrowberg/i2cdevlib/blob/master/Arduino/MPU6050/examples/IMU_Zero/IMU_Zero.ino
    mpu.setXGyroOffset(166);
    mpu.setYGyroOffset(71);
    mpu.setZGyroOffset(4);
    mpu.setXAccelOffset(689);
    mpu.setYAccelOffset(-701);
    mpu.setZAccelOffset(785);

    if (devStatus == 0) {
        // Calibration Time: generate offsets and calibrate our MPU6050
        mpu.CalibrateAccel(6);
        mpu.CalibrateGyro(6);
        // turn on the DMP, now that it's ready
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        return false;
    }
    return true;
}

void Motion::update_rotation(){
    if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        // Quaternion q_rot = rotate(q, rotation_q);
        // mpu.dmpGetEuler(euler, &q_rot);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    }
}

void Motion::reset(){
    rotation_q = q.getNormalized(); 

}

void Motion::get_rotation(float & rot_x, float & rot_y, float & rot_z){
    rot_x = ypr[0];
    rot_y = ypr[1];
    rot_z = ypr[2];
}

void Motion::print_quat(){
	Serial.printf(">3D|pos:S:cube:H:1:W:1:D:2:Q:%f:%f:%f:%f\n", q.x, q.y, q.z, q.w);
	Quaternion r = rotate(q, rotation_q);
    Serial.printf(">3D|rot:S:cube:H:1:W:1:D:2:P:3:0:0:Q:%f:%f:%f:%f\n", r.x, r.y, r.z, r.w);

}

