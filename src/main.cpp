#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_MPU6050.h>

Adafruit_NeoPixel rgb_led(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
Adafruit_MPU6050 mpu;

unsigned long t0 = millis();
bool isConnected = false;


// put function declarations here:
int myFunction(int, int);

float rotX, rotY, rotZ;

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);

  rgb_led.begin();
  rgb_led.setBrightness(70);
  rgb_led.show();

  Serial.begin(115200);
  while (!Serial)
    delay(10);
  if (!mpu.begin()) {
      Serial.println("Failed to find MPU6050 chip");
      while (1) {
        delay(10);
      }
    }
    Serial.println("MPU6050 Found!");

    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");

  rotX = 0;
  rotY = 0;
  rotZ = 0;

  delay(100);

}


int rotation_to_color(float rotation){
  float ratio = rotation / (2*PI);
  Serial.println(ratio);
  if (ratio < 0) ratio = 0;
  else if (ratio > 1) ratio = 1;
  return 255*ratio;
}


unsigned long timer = 0;
float timestep = 0.01;

void loop() {
  timer = millis();

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  rotX += g.gyro.x * timestep;
  rotY += g.gyro.y * timestep;
  rotZ += g.gyro.z * timestep;
  
  rgb_led.setPixelColor(0, rgb_led.Color(rotation_to_color(rotX), rotation_to_color(rotY), rotation_to_color(rotZ)));
  rgb_led.show();
  // Serial.println(rotX);
  delay((timestep*1000) - (millis() - timer));
  
 
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}