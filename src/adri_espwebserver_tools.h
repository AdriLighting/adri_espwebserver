#ifndef ADRI_ESPWEBSERVER_TOOLS_H
	#define ADRI_ESPWEBSERVER_TOOLS_H
	void MDNS_setup(const char * str);
	void MDNS_loop();
	void arduinoOTA_loop();
	void arduinoOTA_setup(const char * hostname);
#endif