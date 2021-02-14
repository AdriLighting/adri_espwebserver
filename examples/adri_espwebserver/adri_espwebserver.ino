#include <adri_espwebserver.h>
#include <adri_espwebserver_tools.h>
#include <adri_wifiConnect.h>
#include <adri_tools_v2.h>
#include <adri_timer.h>
#include "secret.h"

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

void setup() {
	Serial.begin(115200);
	delay(1000);
	fsprintf("\n");

	LittleFS.begin();

	// REGION WIFI
	myWifi 		= new wifiConnect();
	myWifiAp 	= new wifi_credential_ap("");
	myWifiAp->hostname_set(adri_toolsv2Ptr_get()->ch_toString(myWifiHostname));
	wifi_credential_ap_register(myWifiAp);
	if (!wifi_credential_sta_fromSPIFF()) { 	
		wifi_credential_set(
			1, 						
			SECRET_SSID,	
			SECRET_PASS, 			
			"",						
			"",						
			""						
		);
		wifi_credential_set(
			0, 						
			"xxx", 		
			"xxx", 			
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
	myWifiAp->ip_set 					(myWifi->_credential_sta->ip_get());	
	myWifiAp->print 					();	




	myWifi->setup 						();
	if(!myWifiOTA) 	myWifi->MDSN_begin	();
	else 			arduinoOTA_setup	(myWifiHostname);
	wifi_connect_statu 					();
	fsprintf("\n[myWifiConnectDone] : %s\n", adri_toolsv2Ptr_get()->on_time().c_str());

	clientServer.filesystem_ok(true); 		// ENABLED FILE SYSTEM BROWSER
	clientServer.filesystem_set(&LittleFS);

	clientServer.initialize(80);
	clientServer.begin();

	fsprintf("\n[adri_webserver request]\n");
	fsprintf("\t %-15s : index.htm\n", 			fsget(request_handleRoot).c_str());
	fsprintf("\t %-15s : ESP Reset\n", 			fsget(request_espRest).c_str());
	fsprintf("\t %-15s : LittleFS Editor\n",	fsget(request_spiffsEdit).c_str());
	fsprintf("\t %-15s : LittleFS List\n",		fsget(request_spiffsList).c_str());
	fsprintf("\t %-15s : LittleFS Statu\n",		fsget(request_spiffsStatu).c_str());

	// SEE WITH "adri_webserver_reponse" Example for adding request

	socketServer.setup();

	myWifiConnectDone = 1;	
}
void loop()
{
	if (myWifiConnectDone == 1) {
		if(!myWifiOTA) 	myWifi->MDSN_loop();
		else 			arduinoOTA_loop();	

		clientServer.handleLoop();
		socketServer.loop();
	}

}



