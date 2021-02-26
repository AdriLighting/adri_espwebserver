#include <adri_espwebserver.h>
#include <adri_espwebserver_tools.h>

#include <adri_logger.h>
#include <adri_tools_v2.h>
#include <adri_tools_v2_serialMenu.h>
#include <adri_timer.h>
#include <adri_esp_api_reader.h>
#include <adri_timeNtp.h>

#include <adri_wifiConnect.h>
#include "secret.h"

adri_timeNtp 		* myRealTime;

adri_webserver		clientServer(80); // instance adri_webserver 	- ESP8266WebServer _server
adri_socket			socketServer(81); // instance adri_socket 		- WebSocketsServer _socket


fs(request_handleRoot,     	"/");
fs(request_spiffsStatu,     "/status");
fs(request_spiffsList,     	"/list");
fs(request_spiffsEdit,     	"/edit");
fs(request_espRest,			"/request_reset");


wifiConnect 		* myWifi;	// PTR pour unr instance statique "wifiConnect"
wifi_credential_ap	* myWifiAp;	// PTR pour unr instance statique "wifi_credential_ap"
const char 			* myWifiHostname = "MY_ESPWEBSERVER"; 	// AP AND DNS HOSTNAME 
boolean 			myWifiOTA 			= false; 	// ENABLED OTA
int 				myWifiConnectDone	= 0;		// WIFI CONNECT LOOP POSTION 

espUI * _espUI;

adri_toolsV2 			* _tools;
adriToolsv2_serialRead 	* _serial;
adriToolsLogger 		* _looger;
adriApi_meteo 			_adriApi_meteo;

espUI_item * _staticUerItem_meteo;
espUI_item * _staticUerItem_realTime;

String _serial_menu(String cmd, String value);
String _serial_ESPreset(String cmd, String value);
String _serial_freeHeap(String cmd, String value);
String _serial_createMetoCard(String cmd, String value);
String _serial_removeMetoCard(String cmd, String value);
String _serial_sideBardItem(String cmd, String value);

void myTEstFunc(String s ){
	fsprintf("\n[mytest] = %s\n", s.c_str());
}
int myTestBt;

void setup() {
	Serial.begin(115200);
	delay(1000);
	fsprintf("\n");

	LittleFS.begin();

	

	_tools 	= new adri_toolsV2(255);
	_looger = new adriToolsLogger(255);

	// restore_spiffs_factory_files() ;

	// _tools->SPIFFS_printFiles("");

	// REGION WIFI
	myWifi 		= new wifiConnect();
	myWifiAp 	= new wifi_credential_ap("");
	myWifiAp->hostname_set(adri_toolsv2Ptr_get()->ch_toString(myWifiHostname));
	wifi_credential_ap_register(myWifiAp);
	if (!wifi_credential_sta_fromSPIFF()) { 	
		wifi_credential_set(
			0, 						
			SECRET_SSID,	
			SECRET_PASS, 			
			"",						
			"",						
			""						
		);
		wifi_credential_sta_toSpiff();		
	}
	wifi_credential_sta_print();
	myWifi->load_fromSpiif 				();
	myWifi->credential_sta_pos_set 		(0);
	myWifi->connect_set 				(AWC_SETUP);
	myWifi->connectSSID_set 			(AWCS_MULTI);
	myWifi->station_set 				(WIFI_STA);
	myWifi->hostName_set 				(myWifiHostname); 			
	myWifi->setup_id					();						
	myWifiAp->psk_set 					("mywifiappsk");						
	myWifiAp->print 					();	


	myWifi->setup 						();
	if(!myWifiOTA) 	myWifi->MDSN_begin	();
	else 			arduinoOTA_setup	(myWifiHostname);
	wifi_connect_statu 					();
	fsprintf("\n[myWifiConnectDone] : %s\n", adri_toolsv2Ptr_get()->on_time().c_str());

		myRealTime = new adri_timeNtp();
		myRealTime->setup(true);

	_espUI = new espUI(&clientServer._server, &socketServer._socket);

	myTestBt = _espUI->user_button_setup("BtnName", "", myTEstFunc);

	clientServer.filesystem_ok(true); 		// ENABLED FILE SYSTEM BROWSER
	clientServer.filesystem_set(&LittleFS);

	clientServer.initialize(80);
	clientServer.begin();
	socketServer.setup();

	fsprintf("\n[adri_webserver request]\n");
	fsprintf("\t %-15s : index.htm\n", 			fsget(request_handleRoot).c_str());
	fsprintf("\t %-15s : ESP Reset\n", 			fsget(request_espRest).c_str());
	fsprintf("\t %-15s : LittleFS Editor\n",	fsget(request_spiffsEdit).c_str());
	fsprintf("\t %-15s : LittleFS List\n",		fsget(request_spiffsList).c_str());
	fsprintf("\t %-15s : LittleFS Statu\n",		fsget(request_spiffsStatu).c_str());

    _serial = new adriToolsv2_serialRead();
// 							array 1/2	taille
    _serial->cmd_array(		1, 			6); 
//								display name		touche	return 	function
    _serial->cmd_item_add(	1, 	"menu",				"a",	"",		_serial_menu);
    _serial->cmd_item_add(	1, 	"ESPreset",			"z",	"", 	_serial_ESPreset);
    _serial->cmd_item_add(	1, 	"freeHeap",			"e",	"", 	_serial_freeHeap);
    _serial->cmd_item_add(	1, 	"metocard",			"r",	"", 	_serial_createMetoCard);
    _serial->cmd_item_add(	1, 	"meteocardRemove",	"t",	"", 	_serial_removeMetoCard);
    _serial->cmd_item_add(	1, 	"sidebarItemTime",	"y",	"", 	_serial_sideBardItem);

	myWifiConnectDone = 1;	
}
void loop()
{
	if (myWifiConnectDone == 1) {
		if(!myWifiOTA) 	myWifi->MDSN_loop();
		else 			arduinoOTA_loop();	

		clientServer.handleLoop();
		socketServer.loop();
		
		_espUI->loop();
		myRealTime->loop();
	}
	_serial->loop();

}
String _serial_menu(String cmd, String value) {
    adriToolsv2_serialReadPtr_get()->menu();
    return "";
}
String _serial_ESPreset(String cmd, String value){
    ESP.restart();
    return "";
}	
String _serial_freeHeap(String cmd, String value){
	_tools->heap_print();
    return "";
}	


void _staticUerItem_loop(){
	DynamicJsonDocument json(255);
	JsonObject root = json.to<JsonObject>();	

	JsonObject object = root.createNestedObject("upd");	

	time_t time = myRealTime->timeget();
	String timeStr = myRealTime->timetoString(time);

	_staticUerItem_realTime->json(object);
	_staticUerItem_realTime->_value_1 = timeStr;

	String result;
	serializeJson(json, result);

	socketServer.sendTXT(0, result);
}
String _serial_sideBardItem(String cmd, String value){
	_staticUerItem_realTime =_espUI->espUI_createItem(meEspUI_sidebarLblValue, "Heure");
	DynamicJsonDocument json(1024);
	JsonObject root = json.to<JsonObject>();	
	JsonObject object = root.createNestedObject("create");
	JsonObject section = object.createNestedObject("Heure");	
	_staticUerItem_realTime->setJson(section);
	String sSend;
	serializeJson(json, sSend);
	socketServer.sendTXT(0, sSend);

	_espUI->_userLoop = _staticUerItem_loop;

	
	_espUI->user_element_create(myTestBt);
    return "";
}	

String _serial_createMetoCard(String cmd, String value){
	_adriApi_meteo.openweathermap_get(SECRET_OWMID);
	String temp;
	String desc;
	_adriApi_meteo.openweathermap_getKey("temp", temp);	
	_adriApi_meteo.openweathermap_getKey("description", desc);	
	ADRI_LOGV(-1, 2, 0, temp, "","");
	ADRI_LOGV(-1, 2, 0, desc, "","");	
	_staticUerItem_meteo =_espUI->espUI_createItem(meEspUI_card, "meteo");
	_staticUerItem_meteo->_region = "reg_s";
	DynamicJsonDocument json(1024);
	JsonObject root = json.to<JsonObject>();	
	JsonObject object = root.createNestedObject("create");
	JsonObject section = object.createNestedObject("meteo");	
	_staticUerItem_meteo->setJson(section);
	JsonObject data = section.createNestedObject("data");	
	data[F("température")] = temp;
	data[F("description")] = desc;
	String sSend;
	serializeJson(json, sSend);
	socketServer.sendTXT(0, sSend);
    return "";
}	

String _serial_removeMetoCard(String cmd, String value){
	DynamicJsonDocument json(200);
	JsonObject root = json.to<JsonObject>();	

	JsonObject object = root.createNestedObject("remove");	

	String id_1;

	id_1 = _staticUerItem_meteo->_id_1;
	if(id_1 != "") object[id_1] = id_1;

	String result;
	serializeJson(json, result);
	socketServer.sendTXT(0, result);	
    return "";
}	

