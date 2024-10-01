#include <sender.h>

void on_data_sent(const uint8_t *mac_addr, esp_now_send_status_t status) {
	// Serial.print("\r\nLast Packet Send Status:\t");
	// Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

Sender::Sender(/* args */){
}

Sender::~Sender(){
}

void Sender::setup(){
	WiFi.mode(WIFI_STA);
    // Init ESP-NOW
	if (esp_now_init() != ESP_OK) {
		Serial.println("Error initializing ESP-NOW");
		return;
	}

    	// Once ESPNow is successfully Init, we will register for Send CB to
	// get the status of Trasnmitted packet
	esp_now_register_send_cb(on_data_sent);

	// Register peer
	memcpy(baseStationPeerInfo.peer_addr, baseStationAddress, 6);
	baseStationPeerInfo.channel = 0;  
	baseStationPeerInfo.encrypt = false;

    // Add peer        
	if (esp_now_add_peer(&baseStationPeerInfo) != ESP_OK){
		Serial.println("Failed to add peer");
		return;
	}
}

bool Sender::send_message(Message & message)
{
	esp_err_t result = esp_now_send(baseStationAddress, (uint8_t *) &message, sizeof(message));
    if (result == ESP_OK) {
	//   Serial.println("Sent with success");
      return true;
	}
	else {
	  Serial.println("Error sending the data");
      return false;
	}
}

