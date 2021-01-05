#include "adri_espwebserver.h"


#include <adri_tools.h>


#define DBG_OUTPUT_PORT Serial
// #define DEBUG

const char* fsName = "SPIFFS";
SPIFFSConfig fileSystemConfig = SPIFFSConfig();

String unsupportedFiles = String();

static const char TEXT_PLAIN[] PROGMEM = "text/plain";
static const char FS_INIT_ERROR[] PROGMEM = "FS INIT ERROR";
static const char FILE_NOT_FOUND[] PROGMEM = "FileNotFound";

using namespace std::placeholders;

#ifdef ESPUI
	ADRIEsp_ui esp_ui(10); 
#endif
	
adri_socketClient::adri_socketClient()
{
}
void adri_socketClient::setup() {
    // _socket.begin();
    // _socket.onEvent(std::bind(&adri_socketClient::webSocketEvent, this, _1, _2, _3, _4));
	// server address, port and URL
	_socket.begin(_ip, _port, _url);

	// event handler
	_socket.onEvent(std::bind(&adri_socketClient::webSocketEvent, this, _1, _2, _3));

	// try ever 5000 again if connection has failed
	_socket.setReconnectInterval(5000);

	// start heartbeat (optional)
	// ping server every 15000 ms
	// expect pong from server within 3000 ms
	// consider connection disconnected if pong is not received 2 times
	_socket.enableHeartbeat(1200000, 3000, 2);    
}

void adri_socketClient::loop() {
    _socket.loop();
}
void adri_socketClient::webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
	char webSocketClient_buff[length+1];
	switch(type) {
		case WStype_DISCONNECTED:
			break;
		case WStype_CONNECTED: {

		}
			break;
		case WStype_TEXT:
			sprintf(webSocketClient_buff, "%s", payload);
			if (!_webserver_parse)	_user_parse(webSocketClient_buff);			
			else 					parse(String(webSocketClient_buff));
			break;
		case WStype_BIN:
			// hexdump(payload, length);
			break;
        case WStype_PING:
            break;
        case WStype_PONG:
            break;
    }

}

void adri_socketClient::sendTXT( String msg) {
	_socket.sendTXT(msg);
}

void adri_socketClient::parse(String msg) {

	String op 		= literal_value("op", 	msg);
	String value 	= literal_value("cmd", 	msg);
	Serial.printf("\n[parse] op: %s - cmd: %s\n", op.c_str(), msg.c_str());

	#ifdef ADRI_WEBSERVER_REPONSE_H
	for (int i = 0; i < requestReponse_cnt; ++i)
	{ 

		if (requestReponse_array[i]->_protocol != requestProtocol_socket) continue;


		String reponse;

		String parseRequest = value;

		String uri = "/" + op; 

		int pos = -1;
		requestReponse_search(uri, &pos);

		if (pos < 0) continue;

		if (requestReponse_array[pos]->_mod == requestType_name) {

			uri.remove(0, 1);
			parseRequest = "&" + uri + "=";

			if (requestReponse_array[pos]->_parseMod == requestParseCmd_fromFunc) requestReponse_array[pos]->_func(parseRequest); 
			if (requestReponse_array[pos]->_parseMod == requestParseCmd_fromList) requestReponse_pareseUrl_fromList(pos, parseRequest);			

			if (requestReponse_array[pos]->_reponseMod == requestReponseType_fromFunc) requestReponse_array[pos]->_funcReponse(); 
		}

		if (requestReponse_array[pos]->_mod == requestType_wParam) {

			if (requestReponse_array[pos]->_parseMod == requestParseCmd_fromList) requestReponse_pareseUrl_fromList(pos, parseRequest);

			if (requestReponse_array[pos]->_reponseMod == requestReponseType_fromFunc) requestReponse_array[pos]->_funcReponse(); 
		}

		break;

	}
	#endif	
}



adri_socket::adri_socket(int port, String espUi) : _socket(port, "", "arduino")
{
}
adri_socket::adri_socket(int port) : _socket(port, "", "arduino")
{
}

#ifdef ESPUI
void adri_socket::espui_initialize(){esp_ui.socket_initialize(&_socket);}
#endif

void adri_socket::setup() {
    _socket.begin();
    _socket.onEvent(std::bind(&adri_socket::webSocketEvent, this, _1, _2, _3, _4));
}

void adri_socket::loop() {
    _socket.loop();
}

void adri_socket::webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            DBG_OUTPUT_PORT.printf("[%u] Disconnected!\n", num);
            _isConnected = false;
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = _socket.remoteIP(num);
                DBG_OUTPUT_PORT.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
				_num = num;
				// send message to client
				// _socket.sendTXT(num, "Connected");
				
				#ifdef ESPUI
				esp_ui.handleWebpage();
				#endif

				_isConnected = true;
            }
            break;
        case WStype_TEXT:
            DBG_OUTPUT_PORT.printf("[%u] get Text: %s\n", num, payload);
            char buff[100];
            sprintf(buff, "%s", payload);
			_num = num;
			parse(String(buff));
			// parse(payload);
            // send message to client
            // _socket.sendTXT(num, "message here");

            // send data to all connected clients
            // _socket.broadcastTXT("message here");
            break;
        case WStype_BIN:
            // DBG_OUTPUT_PORT.printf("[%u] get binary length: %u\n", num, length);
            hexdump(payload, length);

            // send message to client
            // _socket.sendBIN(num, payload, length);
            break;
    }

}
void adri_socket::broadcastTXT(String msg) {
	_socket.broadcastTXT(msg);
}
void adri_socket::sendTXT(uint8_t num, String msg) {
	_socket.sendTXT(num, msg);
}
boolean adri_socket::isConnected() {
	return _isConnected;
}

void adri_socket::parse(String msg) {

	String op 		= literal_value("op", 	msg);
	String value 	= literal_value("cmd", 	msg);
	Serial.printf("\n[adri_socket::parse] op: %s - cmd: %s\n", op.c_str(), msg.c_str());
	String sOp = "op";

	#ifdef ADRI_WEBSERVER_REPONSE_H
	for (int i = 0; i < requestReponse_cnt; ++i)
	{ 

		if (requestReponse_array[i]->_protocol != requestProtocol_socket) continue;

		String reponse;
		String sOp;

		String parseRequest = value;

		String uri = "/" + op; 

		int pos = -1;
		requestReponse_search(uri, &pos);

		if (pos < 0) continue;

		if (requestReponse_array[pos]->_mod == requestType_name) {

			uri.remove(0, 1);
			parseRequest = "&" + uri + "=";

			if (requestReponse_array[pos]->_parseMod == requestParseCmd_fromFunc) requestReponse_array[pos]->_func(parseRequest); 
			if (requestReponse_array[pos]->_parseMod == requestParseCmd_fromList) requestReponse_pareseUrl_fromList(pos, parseRequest);			

			if (requestReponse_array[pos]->_reponseMod == requestReponseType_fromFunc) requestReponse_array[pos]->_funcReponse(); 
		}

		if (requestReponse_array[pos]->_mod == requestType_wParam) {

			if (requestReponse_array[pos]->_parseMod == requestParseCmd_fromFunc) requestReponse_array[pos]->_func(parseRequest); 
			if (requestReponse_array[pos]->_parseMod == requestParseCmd_fromList) requestReponse_pareseUrl_fromList(pos, parseRequest);

			if (requestReponse_array[pos]->_reponseMod == requestReponseType_fromFunc) requestReponse_array[pos]->_funcReponse(); 
			if (requestReponse_array[pos]->_reponseMod 	== requestReponseType_jsonFromList) {
				requestReponse_reponse_fromList(pos, sOp, &reponse);
				// fsprintf("\nreposne: %d\n%S\n",cnt, reponse.c_str());				
			}	
						
		}

		break;

	}
	#endif	
}


adri_webserver::adri_webserver(int port) : _server(port) {
	_fsOk = true;
}

#ifdef ESPUI
	void adri_webserver::espui_initialize(){esp_ui.server_initialize(&_server);}
#endif

void adri_webserver::filesystem_set(FS * fs){
	_fs = fs;
}
void adri_webserver::filesystem_ok(bool val){
	_fsOk = val;
}

void adri_webserver::request_param_get(String * result){
	String ret = "";
	for (int i = 0; i <= _server.args(); i++) {
		if (_server.argName(i) != "") {
			ret += "&"+_server.argName(i)+"="+_server.arg(i);
		}
	}
	*result=ret;
}
#ifdef ADRI_REPONSE_H

#endif
const char* serverIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";

void adri_webserver::initialize (int port) {

	fileSystemConfig.setAutoFormat(false);
	_fs->setConfig(fileSystemConfig);

	// Debug: dump on console contents of filessytem with no filter and check filenames validity
	Dir dir = _fs->openDir("");
	#ifdef DEBUG
		DBG_OUTPUT_PORT.println("");
		DBG_OUTPUT_PORT.println( F("List of files at root of filesystem:"));
	#endif
	while (dir.next()) {
		String error = checkForUnsupportedPath(dir.fileName());
		String fileInfo = dir.fileName() + (dir.isDirectory() ? " [DIR]" : String(" (") + dir.fileSize() + "b)");
		#ifdef DEBUG
			DBG_OUTPUT_PORT.println(error + fileInfo);
		#endif
		if (error.length() > 0) {
		  unsupportedFiles += error + fileInfo + '\n';
		}
	}
	#ifdef DEBUG
		DBG_OUTPUT_PORT.println();
	#endif
	// Keep the "unsupportedFiles" variable to show it, but clean it up
	unsupportedFiles.replace("\n", "<br/>");
	unsupportedFiles = unsupportedFiles.substring(0, unsupportedFiles.length() - 5);

	// 
	#ifdef ESPUI
		esp_ui.begin();
	#else
		_server.on("/", 		std::bind(&adri_webserver::handleRoot, 	this));
	#endif

	#ifdef ADRI_WEBSERVER_REPONSE_H
	for (int i = 0; i < requestReponse_cnt; ++i)
	{ 

		if (requestReponse_array[i]->_protocol != requestProtocol_http) continue;

		_server.on(requestReponse_array[i]->_name, HTTP_GET, [this]() {

			String reponse;
			String op = "op";

			String parseRequest;
			request_param_get(&parseRequest);

			String uri = ESP8266WebServer::urlDecode(_server.uri()); 

			int pos = 0;
			requestReponse_search(uri, &pos);


			if (requestReponse_array[pos]->_mod == requestType_name) {

				uri.remove(0, 1);
				parseRequest = "&" + uri + "=";

				if (requestReponse_array[pos]->_parseMod == requestParseCmd_fromFunc) requestReponse_array[pos]->_func(parseRequest); 
				if (requestReponse_array[pos]->_parseMod == requestParseCmd_fromList) requestReponse_pareseUrl_fromList(pos, parseRequest);			

				if (requestReponse_array[pos]->_reponseMod == requestReponseType_fromFunc) requestReponse_array[pos]->_funcReponse(); 
				if (requestReponse_array[pos]->_reponseMod == requestReponseType_jsonFromList) {
					boolean cnt = requestReponse_reponse_fromList(pos, op, &reponse);				
					if (cnt) _server.send(200, "application/json", reponse);
					else {
						request_http_noreponse_set_json(&reponse);
						_server.send(200, "application/json", reponse);
					}
				}
				if (requestReponse_array[pos]->_reponseMod == requestReponseType_jsonFromListForce) {
					requestReponse_reponse_fromListForce(pos, op, &reponse);		
					_server.send(200, "application/json", reponse);
				}		
				if (requestReponse_array[pos]->_reponseMod == requestReponseType_SPIFFwebpage) {
					if (!handleFileRead(requestReponse_array[pos]->_SPIFFwebpage)) replyBadRequest(requestReponse_array[pos]->_SPIFFwebpage + " : not found");
				}						
			}

			if (requestReponse_array[pos]->_mod == requestType_wParam) {

				if (requestReponse_array[pos]->_parseMod == requestParseCmd_fromList) requestReponse_pareseUrl_fromList(pos, parseRequest);

				if (requestReponse_array[pos]->_reponseMod == requestReponseType_fromFunc) requestReponse_array[pos]->_funcReponse(); 
				if (requestReponse_array[pos]->_reponseMod == requestReponseType_jsonFromList) {
					boolean cnt = requestReponse_reponse_fromList(pos, op, &reponse);				
					if (cnt) _server.send(200, "application/json", reponse);
					else {
						request_http_noreponse_set_json(&reponse);
						_server.send(200, "application/json", reponse);
					}
				}
				if (requestReponse_array[pos]->_reponseMod == requestReponseType_jsonFromListForce) {
					requestReponse_reponse_fromListForce(pos, op, &reponse);		
					_server.send(200, "application/json", reponse);
				}
				if (requestReponse_array[pos]->_reponseMod == requestReponseType_SPIFFwebpage) {
					if (!handleFileRead(requestReponse_array[pos]->_SPIFFwebpage)) replyBadRequest(requestReponse_array[pos]->_SPIFFwebpage + " : not found");
				}				
			}

			
		});
	}
	#endif


	_server.on("/request_reset", HTTP_GET, [this]() {
		_server.send(200, "text/plain", "reset");
		delay(1000);
		ESP.reset();
	});
	_server.on("/update", HTTP_GET, [this]() {
		_server.sendHeader("Connection", "close");
		_server.send(200, "text/html", serverIndex);
	});
    _server.on("/update", HTTP_POST, [this]() {
      _server.sendHeader("Connection", "close");
      _server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
      ESP.restart();
    }, [this]() {
      HTTPUpload& upload = _server.upload();
      if (upload.status == UPLOAD_FILE_START) {
        Serial.setDebugOutput(true);
        WiFiUDP::stopAll();
        Serial.printf("Update: %s\n", upload.filename.c_str());
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if (!Update.begin(maxSketchSpace)) { //start with max available size
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) { //true to set the size to the current progress
          Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
      }
      yield();
    });

	_server.on("/status", 	HTTP_GET, 		std::bind(&adri_webserver::handleStatus, 		this));
	_server.on("/list", 	HTTP_GET, 		std::bind(&adri_webserver::handleFileList, 		this));

	#ifdef FSB_BROWSER
	_server.on("/edit", 	HTTP_GET, 		std::bind(&adri_webserver::handleGetEdit, 		this));
	_server.on("/edit", 	HTTP_PUT, 		std::bind(&adri_webserver::handleFileCreate, 	this));
	_server.on("/edit", 	HTTP_DELETE, 	std::bind(&adri_webserver::handleFileDelete, 	this));
	_server.on("/edit", 	HTTP_POST, 		std::bind(&adri_webserver::replyOK, this), std::bind(&adri_webserver::handleFileUpload, this));
	#endif

	_server.onNotFound(std::bind(&adri_webserver::handleNotFound, this));
	
	// Serial.println("HTTP _server started on port ");
}
void adri_webserver::getUrl(String * uri) {
	* uri = ESP8266WebServer::urlDecode(_server.uri());
}
void adri_webserver::handleLoop() {
	_server.handleClient();
}
void adri_webserver::begin() {
	_server.begin();
}
////////////////////////////////
// Utils to return HTTP codes, and determine content-type

void adri_webserver::replyOK() {
  _server.send(200, FPSTR(TEXT_PLAIN), "");
}

void adri_webserver::replyOKWithMsg(String msg) {
  _server.send(200, FPSTR(TEXT_PLAIN), msg);
}

void adri_webserver::replyNotFound(String msg) {
  _server.send(404, FPSTR(TEXT_PLAIN), msg);
}

void adri_webserver::replyBadRequest(String msg) {
	#ifdef DEBUG
		DBG_OUTPUT_PORT.println(msg);
	#endif
	_server.send(400, FPSTR(TEXT_PLAIN), msg + "\r\n");
}

void adri_webserver::replyServerError(String msg) {
	#ifdef DEBUG
		DBG_OUTPUT_PORT.println(msg);
	#endif
	_server.send(500, FPSTR(TEXT_PLAIN), msg + "\r\n");
}


void adri_webserver::handleRoot() {

	if (!_fsOk) {
		return replyServerError(FPSTR(FS_INIT_ERROR));
	}

	String uri = ESP8266WebServer::urlDecode(_server.uri()); // required to read paths with blanks

	if (handleFileRead(uri)) {
		return;
	}
	return replyNotFound(FPSTR(FILE_NOT_FOUND));

/*
	// handleFileRead("/index.htm");
 	 // _server.send(200, "text/html", "/index.htm");
 	String path = "/index.htm";
 	#ifdef DEBUG
		DBG_OUTPUT_PORT.println(String("\n[handleRoot]: ") + path);
	#endif
	if (!_fsOk) {
		replyServerError(FPSTR(FS_INIT_ERROR));
		return ;
	}

	if (path.endsWith("/")) {
		path += "index.htm";
	}

	String contentType = "text/html";
	// if (_server.hasArg("download")) {
	// 	contentType = F("application/octet-stream");
	// } else {
	// 	contentType = mime::getContentType(path);
	// }

	if (!_fs->exists(path)) {
		// File not found, try gzip version
		path = path + ".gz";
	}
	if (_fs->exists(path)) {
		File file = _fs->open(path, "r");
		if (_server.streamFile(file, contentType) != file.size()) {
			#ifdef DEBUG
				DBG_OUTPUT_PORT.println(String("\n[Sent less data than expected!]\n"));
			#endif
		}
		file.close();
		return ;
	}

	return ;
*/
}

/*
	 The "Not Found" handler catches all URI not explicitely declared in code
	 First try to find and return the requested file from the filesystem,
	 and if it fails, return a 404 page with debug information
*/
void adri_webserver::handleNotFound() {
	if (!_fsOk) {
		return replyServerError(FPSTR(FS_INIT_ERROR));
	}

	String uri = ESP8266WebServer::urlDecode(_server.uri()); // required to read paths with blanks

	if (handleFileRead(uri)) {
		return;
	}

	// Dump debug data
	// String message;
	// message.reserve(100);
	// message = F("Error: File not found\n\nURI: ");
	// message += uri;
	// message += F("\nMethod: ");
	// message += (_server.method() == HTTP_GET) ? "GET" : "POST";
	// message += F("\nArguments: ");
	// message += _server.args();
	// message += '\n';
	// for (uint8_t i = 0; i < _server.args(); i++) {
	// 	message += F(" NAME:");
	// 	message += _server.argName(i);
	// 	message += F("\n VALUE:");
	// 	message += _server.arg(i);
	// 	message += '\n';
	// }
	// message += "path=";
	// message += _server.arg("path");
	// message += '\n';
	// #ifdef DEBUG
	// 	DBG_OUTPUT_PORT.print(message);
	// #endif 
	return replyNotFound(FPSTR(FILE_NOT_FOUND));	
	// return replyNotFound(message);
}

/*
	 Read the given file from the filesystem and stream it back to the client
*/
bool adri_webserver::handleFileRead(String path) {
	#ifdef DEBUG
		DBG_OUTPUT_PORT.println(String("\n[handleFileRead]: ") + path);
	#endif	
	if (!_fsOk) {
		replyServerError(FPSTR(FS_INIT_ERROR));
		return true;
	}

	if (path.endsWith("/")) {
		path += "index.htm";
	}

	String contentType;
	if (_server.hasArg("download")) {
		contentType = F("application/octet-stream");
	} else {
		contentType = mime::getContentType(path);
	}

	if (!_fs->exists(path)) {
		// File not found, try gzip version
		path = path + ".gz";
	}
	if (_fs->exists(path)) {
		File file = _fs->open(path, "r");
		if (_server.streamFile(file, contentType) != file.size()) {
			#ifdef DEBUG
				DBG_OUTPUT_PORT.println("Sent less data than expected!");
			#endif
		}
		file.close();
		return true;
	}

	return false;
}

////////////////////////////////
// Request handlers
/*
	 Return the FS type, status and size info
*/
void adri_webserver::handleJson(String json) {
	#ifdef DEBUG
		DBG_OUTPUT_PORT.println("handleJson");
	#endif

	_server.send(200, "application/json", json);
}

/*
	 Return the FS type, status and size info
*/

String adri_webserver::sendNetworkStatus()
{
	// if (isAdmin(request) == false) return;
	uint8_t mac[6];
	char macStr[18] = { 0 };
	WiFi.macAddress(mac);
	sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	String state = "N/A";
	String Networks = "";
	if (WiFi.status() == 0) state = "Idle";
	else if (WiFi.status() == 1) state = "NO SSID AVAILBLE";
	else if (WiFi.status() == 2) state = "SCAN COMPLETED";
	else if (WiFi.status() == 3) state = "CONNECTED";
	else if (WiFi.status() == 4) state = "CONNECT FAILED";
	else if (WiFi.status() == 5) state = "CONNECTION LOST";
	else if (WiFi.status() == 6) state = "DISCONNECTED";

	Networks = "";  //future to scan and show networks async

	String wifilist = "";
	wifilist += "WiFi State: " + state + "<br>";
	wifilist += "Scanned Networks <br>" + Networks + "<br>";

	String values = "<body> ";
	values += "SSID            	:  " + (String)WiFi.SSID()          + "<br>";
	values += "IP Address      	:  " + (String)WiFi.localIP()[0]    + "." + (String)WiFi.localIP()[1]      + "." + (String)WiFi.localIP()[2]      + "." + (String)WiFi.localIP()[3]      + "<br>";
	values += "Wifi Gateway    	:  " + (String)WiFi.gatewayIP()[0]  + "." + (String)WiFi.gatewayIP()[1]    + "." + (String)WiFi.gatewayIP()[2]    + "." + (String)WiFi.gatewayIP()[3]    + "<br>";
	values += "NetMask         	:  " + (String)WiFi.subnetMask()[0] + "." + (String)WiFi.subnetMask()[1]   + "." + (String)WiFi.subnetMask()[2]   + "." + (String)WiFi.subnetMask()[3]   + "<br>";
	values += "Mac Address     	>  " + String(macStr) + "<br>";
	// values += "NTP Time       :   " + String(hour()) + ":" + String(minute()) + ":" + String(second()) + " " + String(year()) + "-" + String(month()) + "-" + String(day()) + "<br>";
	values += "Server Uptime   	:  " + String(millis() / 60000)     + " minutes" + "<br>";
	values += "Server Heap     	:  " + String(ESP.getFreeHeap())    + "<br>";
	if (_fsOk) {
	FSInfo fs_info;
	_fs->info(fs_info);
	values += "SPIIFS<br>";
	values += "TotalBytes		:  " + String(fs_info.totalBytes) + "<br>";
	values += "UsedBytes		:  " + String(fs_info.usedBytes) + "<br>";
	values += "FreeBytes		:  " + String(fs_info.totalBytes-fs_info.usedBytes) + "<br>";
	}
	values += wifilist;
	values += " <input action=\"action\" type=\"button\" value=\"Back\" onclick=\"history.go(-1);\" style=\"width: 100px; height: 50px;\" /> </body> ";
	return values;
}
void adri_webserver::handleStatus() {
	// #ifdef DEBUG
	// 	DBG_OUTPUT_PORT.println("handleStatus");
	// #endif
	// FSInfo fs_info;
	// String json;
	// json.reserve(128);

	// json = "{\"type\":\"";
	// json += fsName;
	// json += "\", \"isOk\":";
	// if (_fsOk) {
	// 	_fs->info(fs_info);
	// 	json += F("\"true\", \"totalBytes\":\"");
	// 	json += fs_info.totalBytes;
	// 	json += F("\", \"usedBytes\":\"");
	// 	json += fs_info.usedBytes;
	// 	json += "\"";
	// } else {
	// 	json += "\"false\"";
	// }
	// json += F(",\"unsupportedFiles\":\"");
	// json += unsupportedFiles;
	// json += "\"}";

	// _server.send(200, "application/json", json);

	_server.send(200, "text/html", sendNetworkStatus());
}

/*
	 Return the list of files in the directory specified by the "dir" query string parameter.
	 Also demonstrates the use of chuncked responses.
*/
void adri_webserver::handleFileList() {
	if (!_fsOk) {
		return replyServerError(FPSTR(FS_INIT_ERROR));
	}

	if (!_server.hasArg("dir")) {
		return replyBadRequest(F("DIR ARG MISSING"));
	}

	String path = _server.arg("dir");
	// if ((path != "/") || (!_fs->exists(path))) {

	#ifdef DEBUG
		DBG_OUTPUT_PORT.println(String("handleFileList: ") + path);
	#endif
	Dir dir = _fs->openDir(path);
	path.clear();

	// use HTTP/1.1 Chunked response to avoid building a huge temporary string
	if (!_server.chunkedResponseModeStart(200, "application/json")) {
		_server.send(505, F("text/html"), F("HTTP1.1 required"));
		return;
	}

	// use the same string for every line
	String output;
	output.reserve(64);
	while (dir.next()) {
		#ifdef USE_SPIFFS
				String error = checkForUnsupportedPath(dir.fileName());
				if (error.length() > 0) {
					#ifdef DEBUG
						DBG_OUTPUT_PORT.println(String("Ignoring ") + error + dir.fileName());
					#endif
					continue;
				}
		#endif
		if (output.length()) {
			// send string from previous iteration
			// as an HTTP chunk
			_server.sendContent(output);
			output = ',';
		} else {
			output = '[';
		}

		output += "{\"type\":\"";
		if (dir.isDirectory()) {
			output += "dir";
		} else {
			output += F("file\",\"size\":\"");
			output += dir.fileSize();
		}

		output += F("\",\"name\":\"");
		// Always return names without leading "/"
		if (dir.fileName()[0] == '/') {
			output += &(dir.fileName()[1]);
		} else {
			output += dir.fileName();
		}

		output += "\"}";
	}

	// send last string
	output += "]";
	_server.sendContent(output);
	_server.chunkedResponseFinalize();
}


#ifdef FSB_BROWSER
/*
	 As some FS (e.g. LittleFS) delete the parent folder when the last child has been removed,
	 return the path of the closest parent still existing
*/
String adri_webserver::lastExistingParent(String path) {
	while (!path.isEmpty() && !_fs->exists(path)) {
		if (path.lastIndexOf('/') > 0) {
			path = path.substring(0, path.lastIndexOf('/'));
		} else {
			path = String();  // No slash => the top folder does not exist
		}
	}
	#ifdef DEBUG
		DBG_OUTPUT_PORT.println(String("Last existing parent: ") + path);
	#endif
	return path;
}

/*
	 Handle the creation/rename of a new file
	 Operation      | req.responseText
	 ---------------+--------------------------------------------------------------
	 Create file    | parent of created file
	 Create folder  | parent of created folder
	 Rename file    | parent of source file
	 Move file      | parent of source file, or remaining ancestor
	 Rename folder  | parent of source folder
	 Move folder    | parent of source folder, or remaining ancestor
*/
void adri_webserver::handleFileCreate() {
	if (!_fsOk) {
		return replyServerError(FPSTR(FS_INIT_ERROR));
	}

	String path = _server.arg("path");
	if (path.isEmpty()) {
		return replyBadRequest(F("PATH ARG MISSING"));
	}

#ifdef USE_SPIFFS
	if (checkForUnsupportedPath(path).length() > 0) {
		return replyServerError(F("INVALID FILENAME"));
	}
#endif

	if (path == "/") {
		return replyBadRequest("BAD PATH");
	}
	if (_fs->exists(path)) {
		return replyBadRequest(F("PATH FILE EXISTS"));
	}

	String src = _server.arg("src");
	if (src.isEmpty()) {
		// No source specified: creation
		#ifdef DEBUG
			DBG_OUTPUT_PORT.println(String("handleFileCreate: ") + path);
		#endif
		if (path.endsWith("/")) {
			// Create a folder
			path.remove(path.length() - 1);
			if (!_fs->mkdir(path)) {
				return replyServerError(F("MKDIR FAILED"));
			}
		} else {
			// Create a file
			File file = _fs->open(path, "w");
			if (file) {
				file.write((const char *)0);
				file.close();
			} else {
				return replyServerError(F("CREATE FAILED"));
			}
		}
		if (path.lastIndexOf('/') > -1) {
			path = path.substring(0, path.lastIndexOf('/'));
		}
		replyOKWithMsg(path);
	} else {
		// Source specified: rename
		if (src == "/") {
			return replyBadRequest("BAD SRC");
		}
		if (!_fs->exists(src)) {
			return replyBadRequest(F("SRC FILE NOT FOUND"));
		}

		#ifdef DEBUG
			DBG_OUTPUT_PORT.println(String("handleFileCreate: ") + path + " from " + src);
		#endif

		if (path.endsWith("/")) {
			path.remove(path.length() - 1);
		}
		if (src.endsWith("/")) {
			src.remove(src.length() - 1);
		}
		if (!_fs->rename(src, path)) {
			return replyServerError(F("RENAME FAILED"));
		}
		replyOKWithMsg(lastExistingParent(src));
	}
}


/*
	 Delete the file or folder designed by the given path.
	 If it's a file, delete it.
	 If it's a folder, delete all nested contents first then the folder itself

	 IMPORTANT NOTE: using recursion is generally not recommended on embedded devices and can lead to crashes (stack overflow errors).
	 This use is just for demonstration purpose, and FSBrowser might crash in case of deeply nested filesystems.
	 Please don't do this on a production system.
*/
void adri_webserver::deleteRecursive(String path) {
	File file = _fs->open(path, "r");
	bool isDir = file.isDirectory();
	file.close();

	// If it's a plain file, delete it
	if (!isDir) {
		_fs->remove(path);
		return;
	}

	// Otherwise delete its contents first
	Dir dir = _fs->openDir(path);

	while (dir.next()) {
		deleteRecursive(path + '/' + dir.fileName());
	}

	// Then delete the folder itself
	_fs->rmdir(path);
}


/*
	 Handle a file deletion request
	 Operation      | req.responseText
	 ---------------+--------------------------------------------------------------
	 Delete file    | parent of deleted file, or remaining ancestor
	 Delete folder  | parent of deleted folder, or remaining ancestor
*/
void adri_webserver::handleFileDelete() {
	if (!_fsOk) {
		return replyServerError(FPSTR(FS_INIT_ERROR));
	}

	String path = _server.arg(0);
	if (path.isEmpty() || path == "/") {
		return replyBadRequest("BAD PATH");
	}
	#ifdef DEBUG
		DBG_OUTPUT_PORT.println(String("handleFileDelete: ") + path);
	#endif
	if (!_fs->exists(path)) {
		return replyNotFound(FPSTR(FILE_NOT_FOUND));
	}
	deleteRecursive(path);

	replyOKWithMsg(lastExistingParent(path));
}

/*
	 Handle a file upload request
*/
void adri_webserver::handleFileUpload() {
	if (!_fsOk) {
		return replyServerError(FPSTR(FS_INIT_ERROR));
	}
	if (_server.uri() != "/edit") {
		return;
	}
	HTTPUpload& upload = _server.upload();
	if (upload.status == UPLOAD_FILE_START) {
		String filename = upload.filename;
		// Make sure paths always start with "/"
		if (!filename.startsWith("/")) {
			filename = "/" + filename;
		}
		#ifdef DEBUG
			DBG_OUTPUT_PORT.println(String("handleFileUpload Name: ") + filename);
		#endif
		_fsUploadFile = _fs->open(filename, "w");
		if (!_fsUploadFile) {
			return replyServerError(F("CREATE FAILED"));
		}
		#ifdef DEBUG
			DBG_OUTPUT_PORT.println(String("Upload: START, filename: ") + filename);
		#endif
	} else if (upload.status == UPLOAD_FILE_WRITE) {
		if (_fsUploadFile) {
			size_t bytesWritten = _fsUploadFile.write(upload.buf, upload.currentSize);
			if (bytesWritten != upload.currentSize) {
				return replyServerError(F("WRITE FAILED"));
			}
		}
		#ifdef DEBUG
			DBG_OUTPUT_PORT.println(String("Upload: WRITE, Bytes: ") + upload.currentSize);
		#endif

	} else if (upload.status == UPLOAD_FILE_END) {
		if (_fsUploadFile) {
			_fsUploadFile.close();
		}
		#ifdef DEBUG
			DBG_OUTPUT_PORT.println(String("Upload: END, Size: ") + upload.totalSize);
		#endif
	}
}

/*
	 This specific handler returns the index.htm (or a gzipped version) from the /edit folder.
	 If the file is not present but the flag INCLUDE_FALLBACK_INDEX_HTM has been set, falls back to the version
	 embedded in the program code.
	 Otherwise, fails with a 404 page with debug information
*/
void adri_webserver::handleGetEdit() {
	if (handleFileRead(F("/edit/index.htm"))) {
		return;
	}

#ifdef INCLUDE_FALLBACK_INDEX_HTM
	_server.sendHeader(F("Content-Encoding"), "gzip");
	_server.send(200, "text/html", index_htm_gz, index_htm_gz_len);
#else
	replyNotFound(FPSTR(FILE_NOT_FOUND));
#endif
}

#endif

/*
	 Checks filename for character combinations that are not supported by FSBrowser (alhtough valid on SPIFFS).
	 Returns an empty String if supported, or detail of error(s) if unsupported
*/
String checkForUnsupportedPath(String filename) {
	String error = String();
	if (!filename.startsWith("/")) {
		error += F("!NO_LEADING_SLASH! ");
	}
	if (filename.indexOf("//") != -1) {
		error += F("!DOUBLE_SLASH! ");
	}
	if (filename.endsWith("/")) {
		error += F("!TRAILING_SLASH! ");
	}
	return error;
}