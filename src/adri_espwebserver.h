#ifndef ADRI_ESPWEBSERVER_H
	#define ADRI_ESPWEBSERVER_H

	#include <arduino.h>

	#include <ESP8266WiFi.h>
	#include <WiFiClient.h>
	#include <ESP8266WebServer.h>
	#include <ESP8266mDNS.h>
	#include <SPI.h>

	#include <WebSockets.h>           
	#include <WebSocketsServer.h>
	#include <WebSocketsClient.h>

	#include <ArduinoOTA.h>

	#include <adri_webserver_reponselist.h>
	#include <adri_webserver_parsecommand.h>

	#include <adri_webserver_reponse.h>

	#define FSB_BROWSER

	String checkForUnsupportedPath(String filename) ;


	class adri_webserver
	{
	private:
        FS * 			_fs;
        bool 			_fsOk;
		File 			_fsUploadFile;

		void 	getUrl(String * uri);
        void 	handleRoot();
        void 	handleNotFound();
	    bool 	handleFileRead(String path);
        void 	handleStatus();  
        void 	handleFileList();  

        #ifdef FSB_BROWSER
        void 	handleFileUpload();  
        void 	handleFileDelete();  
        void 	handleFileCreate();  
        String 	lastExistingParent(String path);
        void 	deleteRecursive(String path);
        void	handleGetEdit();
        #endif
        

    public:
  		ESP8266WebServer _server; 

    	adri_webserver(int port);

        void 	handleLoop();  
        void 	request_param_get(String * result);
        void 	initialize(int port);
        void 	filesystem_set(FS * fs);  
        void 	filesystem_ok(bool val);  

		void 	replyOK();
		void 	replyOKWithMsg(String msg);
		void 	replyNotFound(String msg);
		void 	replyBadRequest(String msg);
		void 	replyServerError(String msg);        
	};

	class adri_socket
	{

	public:
		WebSocketsServer 		_socket;		
		boolean isConnected 	= false;
		uint8_t _num			= 0;
		adri_socket(int port);
		void parse(String);
		void setup();
		void loop();
		void broadcastTXT(String);
		void sendTXT(uint8_t, String);
		void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
	};

	class adri_socketClient
	{

	public:
		WebSocketsClient	_socket;		
		boolean 			isConnected = false;
		String 				_ip 		= "192.168.0.92";
		const char * 		_url 		= "/";
		const char * 		_protocol 	= "arduino";
		uint16_t 			_port 		= 81;
		uint8_t 			_num 		= 0;
		boolean 			_webserver_parse = true;

		adri_socketClient();
		void parse(String);
		void (*_user_parse)(String value);

		void setup();
		void loop();
		void sendTXT(String);
		void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
	};	
#endif
