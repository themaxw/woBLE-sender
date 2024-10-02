#include <motion.h>

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
}

Motion::Motion(){
}



Motion::~Motion(){
}


// rotation has to be normalized
Quaternion rotate(Quaternion rotation, Quaternion q2){
    Quaternion q = rotation.getProduct(q2);
    return q.getProduct(rotation.getConjugate());
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
        Quaternion q_rot = rotation_q.getProduct(q);
        mpu.dmpGetEuler(euler, &q_rot);

    }
}

void Motion::reset_rotation(){
    rotation_q = q.getNormalized().getConjugate(); 

}

void Motion::get_rotation(float & rot_x, float & rot_y, float & rot_z){
    rot_x = euler[0];
    rot_y = euler[1];
    rot_z = euler[2];
}

void Motion::print_quat(){
	// Serial.printf(">3D|pos:S:cube:H:1:W:1:D:2:Q:%f:%f:%f:%f\n", q.x, q.y, q.z, q.w);
	// Quaternion r = rotate( rotation_q, q);
    // Serial.printf(">3D|rot:S:cube:H:1:W:1:D:2:P:3:0:0:Q:%f:%f:%f:%f\n", r.x, r.y, r.z, r.w);
	// Quaternion i = rotation_q.getProduct(q);
    // Serial.printf(">3D|inv:S:cube:H:1:W:1:D:2:P:-3:0:0:Q:%f:%f:%f:%f\n", i.x, i.y, i.z, i.w);



}

