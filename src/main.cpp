#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_MPU6050.h>
#include <esp_now.h>
#include <WiFi.h>
#include <Message.h>

Adafruit_NeoPixel rgb_led(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
Adafruit_MPU6050 mpu;

unsigned long timer = 0;
float timestep = 0.01;

float rotX, rotY, rotZ;
Message message;

uint8_t baseStationAddress[] = {0x4C, 0x75, 0x25, 0x37, 0x2D, 0xB1};
esp_now_peer_info_t baseStationPeerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // Serial.print("\r\nLast Packet Send Status:\t");
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);


  rgb_led.begin();
  rgb_led.setBrightness(70);
  rgb_led.show();

  Serial.begin(115200);
  

  WiFi.mode(WIFI_STA);
  
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(baseStationPeerInfo.peer_addr, baseStationAddress, 6);
  baseStationPeerInfo.channel = 0;  
  baseStationPeerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&baseStationPeerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  if (!mpu.begin()) {
    //TODO error state irgendwie
    Serial.println("Failed to find MPU6050 chip");
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  
  rotX = 0;
  rotY = 0;
  rotZ = 0;

  delay(100);

}


int rotation_to_color(float rotation){
  float ratio = rotation / (2*PI);
  if (ratio < 0) ratio = 0;
  else if (ratio > 1) ratio = 1;
  return 255*ratio;
}


void loop() {
  timer = millis();

  float measuredvbat = analogReadMilliVolts(BATT_MONITOR);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat /= 1000; // convert to volts!
  //TODO calculate battery charge from Vbat
  // Serial.print("Battery Voltage: ");
  // Serial.println(measuredvbat);



  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  rotX += g.gyro.x * timestep;
  rotY += g.gyro.y * timestep;
  rotZ += g.gyro.z * timestep;
  
  rgb_led.setPixelColor(0, rgb_led.Color(rotation_to_color(rotX), rotation_to_color(rotY), rotation_to_color(rotZ)));
  rgb_led.show();
  // Serial.println(rotX);
  message.rot_x = rotX;
  message.rot_y = rotY;
  message.rot_z = rotZ;
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(baseStationAddress, (uint8_t *) &message, sizeof(message));
   
  // if (result == ESP_OK) {
  //   Serial.println("Sent with success");
  // }
  // else {
  //   Serial.println("Error sending the data");
  // }

  delay((timestep*1000) - (millis() - timer));
  
 
}
