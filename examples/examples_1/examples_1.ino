#include <adri_espwebserver.h>
#include <adri_wifi.h>

#define DEVICENAME String("HELLO")

adri_webserver 	clientServer(80); 
adri_socket 	socketServer(81); 

void setup()
{
    Serial.begin(115200);

	bool fsOK = SPIFFS.begin();	

	Serial.println("");
// WiFi.printDiag(Serial) ;
//   WiFi.persistent(false);


	wfifi_getID_toSPIFF("freebox_123_EXT", "phcaadax", "", ADWIFI_STATION);
	wifi_setup(DEVICENAME);
	#ifdef DEBUG
		String s;
		s = "\nwifi_print_networkInfo\n";
		wifi_print_networkInfo(&s);  
		s+= network_info(DEVICENAME);
		Serial.printf("%s\n", s.c_str());
	#endif

 	#ifdef ADRI_WEBSERVER_REPONSE_H
	int pos = requestReponse_initialize("/hello");
	requestReponse_protocol(pos, requestProtocol_http);
	requestReponse_mod_set(pos, requestType_name);
	requestReponse_reponseMod(pos, requestReponseType_none);
	requestReponse_parseMod(pos, requestParseCmd_fromFunc);
	requestReponse_func(pos, cmd_test2);
	#endif
	
	clientServer.filesystem_ok(fsOK);
	if (fsOK) clientServer.filesystem_set(&SPIFFS);
	
	clientServer.initialize(80);
	socketServer.setup();
	

}
void loop()
{
	clientServer.handleLoop();
	socketServer.loop();
}
void cmd_test2(){
	Serial.println("cmd_test2");
	clientServer.replyOKWithMsg("HELL WORLD");
}