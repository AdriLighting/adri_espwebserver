#ifndef ESP_UI_H
#define ESP_UI_H
#include "config.h"
#ifdef ESPUI
	#include <arduino.h>

	#include <ESP8266WiFi.h>
	#include <WiFiClient.h>
	#include <ESP8266WebServer.h>
	#include <ESP8266mDNS.h>

	#include <WebSockets.h>           
	#include <WebSocketsServer.h>
	#include <WebSocketsClient.h>
	
	#include <ArduinoJson.h>

	#include <adri_timer.h>

	#include "esp_ui_def.h"
	#include "esp_ui_item.h"

	class espUI {
	private:
		ESP8266WebServer * _server; 
		WebSocketsServer * _webSocket; 	

		int 				_element_pos = 0;

		espUI_item 			* _sidebarOnTime 	= nullptr;
		espUI_item 			* _sidebarFreeHeap 	= nullptr;
		espUI_item 			* _cardEspCoreInfos = nullptr;
		espUI_item 			* _cardEspFlashnfos = nullptr;
		espUI_item 			* _card_meteo 		= nullptr;
		espUI_item 			* _sidebarRealTimer = nullptr;
		espUI_item 			* _button_espReset 	= nullptr;
		espUI_item 			* _wifiSetting[10];

		adri_timer 			* _timer_upd = nullptr;
		adri_timer 			* _timer_upd_2 = nullptr;

	public:
		espUI_userLoop  	_userLoop	= NULL;
		int 				_region		= 0;
		int 				_upd_mod	= 0;

		espUI(ESP8266WebServer * server, WebSocketsServer * socket);
		espUI_item * espUI_createItem(mEspUI_elments mod, String s);

		void handleSCSS();
		void handleJS_CORE();
		void handleJS();
		void handleRoot();	
		void handleRootRegister();
		void handleAssetRegister();

		void sidebar_create_label();
		void sidebar_update_label();

		void card_create_espInfos();
		void inputText_create_wifiSetting();

		boolean commandSplit(String command, String value, String & result);
		void button_create_espRest();
		void button_handle_espRest(String ctrlId);
		void home_create();
		void tools_create();
		void api_create();

		void removeElements_system();
		void removeElements_wifiSetting();
		void removeElements_home();
		void removeElements_tools();
		void removeElements_api();
		void removeElements();

		void wifiSetting();
		void loop();

		int user_button_setup(String name, String btName, espUIitem_handle f);
		void user_button_create(int name);
		void user_button_loop(String name);
	};
	
	espUI * espUI_ptrGet();
	
	// void restore_spiffs_factory_files() ;

	// extern espUIitem_handle _tesftfunc;
	// void _testfuncwparm(espUIitem_handle v);


#endif // ESP_UI_H
#endif // ESP_UI_H
