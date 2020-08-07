#include "adri_espwebserver_tools.h"

#if defined(ARDUINO_ARCH_ESP32)
    #include <WiFi.h>
#elif defined(ARDUINO_ARCH_ESP8266)
    #include <ESP8266WiFi.h>
    #include <ESP8266mDNS.h>
    #include <ESP8266WiFiType.h>
#endif
#include <ArduinoOTA.h>

void arduinoOTA_setup(const char * hostname) {
	ArduinoOTA.onStart([]() { Serial.println("Update Start"); });
	ArduinoOTA.onEnd([]() 	{ Serial.println("Update End"); });
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
	char p[32];
	sprintf(p, "Progress: %u%%\n", (progress/(total/100)));
	Serial.println(p);
	});
	ArduinoOTA.onError([](ota_error_t error) {
	if(error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
		else if(error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
		else if(error == OTA_CONNECT_ERROR) Serial.println("onnect Failed");
		else if(error == OTA_RECEIVE_ERROR) Serial.println("Recieve Failed");
		else if(error == OTA_END_ERROR) Serial.println("End Failed");
	});
	ArduinoOTA.setHostname(hostname);
	ArduinoOTA.begin();
}
void arduinoOTA_loop(){
	ArduinoOTA.handle();
}
void MDNS_setup(const char * name){
    if (!MDNS.begin(name)) {
        Serial.println("Error setting up MDNS responder!");
        while (1) {
            delay(1000);
        }
    }
    Serial.println("mDNS responder started");
    // Add service to MDNS-SD
    MDNS.addService("http", "tcp", 80);    
}
void MDNS_loop() {
    MDNS.update();
}
