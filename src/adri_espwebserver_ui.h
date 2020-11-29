#ifndef ADRI_ESPWEBSERVER_UI
#define ADRI_ESPWEBSERVER_UI 
#include "config.h"
#ifdef ESPUI
#include <Arduino.h>
#include "stdlib_noniso.h"
#include <FS.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <WebSocketsServer.h>
#include "Hash.h"

class ADRIEsp_ui  {
	ESP8266WebServer * _server; 
	WebSocketsServer * _webSocket; 
	bool CDN = false;
public:
 	const char* tbuttontitle[10];   	// Stores Toggle Button Titles - MAX 10
  	const char* label_title[10];		// Stores Label Titles - MAX 10
  	const char* label_value[10];		// Stores Label Values - MAX 10
  	const char* ui_title = "adiEspWebserver"; 	// Store UI Title and Header Name
  	int 		l_index;    			// Calculate How Many Labels
  	int 		tb_index;   			// Calculate How Many Toggle Buttons


	ADRIEsp_ui(int prot);

	void server_initialize(ESP8266WebServer * server);
	void socket_initialize(WebSocketsServer * socket);
	void begin();

	void handleWebpage();

	void handleRoot();      // Handle Index HTML
	void handleNCSS();      // Handle Normalize CSS
	void handleSCSS();      // Handle Main Style CSS
	void handleJS();        // Handle JQuery
	// void handleNotFound();  // Handle Page Not-Found
	void handleSockets();   // Handle Sockets Script


	void detectCDN(bool _autoState);
	void title(const char* _title);
	void label(const char* label_name,const char*  label_val);
};
#endif
#endif