#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include <Message.h>

#include <motion.h>
#include <sender.h>

Adafruit_NeoPixel rgb_led(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

unsigned long timer = 0;
float time_between_messages_ms = 20;

Message message;
Motion motion;
Sender sender;
int prev_button_state = HIGH;

void setup() {

	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(BUTTON, INPUT_PULLUP);

	rgb_led.begin();
	rgb_led.setBrightness(70);
	rgb_led.show();

	Serial.begin(115200);
	
    motion.setup();
    sender.setup();
	delay(100);

}


int rotation_to_color(float rotation){
	float ratio = rotation / (2*PI);
	if (ratio < 0) ratio = 0;
	else if (ratio > 1) ratio = 1;
	return 255*ratio;
}

float read_bat_voltage(){
    float measuredvbat = analogReadMilliVolts(BATT_MONITOR);
	measuredvbat *= 2;    // we divided by 2, so multiply back
	measuredvbat /= 1000; // convert to volts!
    return measuredvbat;

	//TODO calculate battery charge from Vbat
	// Serial.print("Battery Voltage: ");
	// Serial.println(measuredvbat);
}

void loop() {


    motion.update_rotation();

	rgb_led.setPixelColor(0, rgb_led.Color(rotation_to_color(message.rot_x), rotation_to_color(message.rot_y), rotation_to_color(message.rot_z)));
	rgb_led.show();
	if(millis() > timer + time_between_messages_ms){
    	motion.get_rotation(message.rot_x, message.rot_y, message.rot_z);

		// Serial.printf(">rotx:%f\n>roty:%f\n>rotz:%f\n", message.rot_x, message.rot_y, message.rot_z);
		motion.print_quat();
		sender.send_message(message);
		timer = millis();
	}
	int button_state = digitalRead(BUTTON);

	if (button_state == LOW && prev_button_state == HIGH){
		Serial.println("Resetting...");
		motion.reset();
	}
	prev_button_state = button_state;
 
}
