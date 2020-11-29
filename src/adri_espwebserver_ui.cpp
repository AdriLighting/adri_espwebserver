#include "adri_espwebserver_ui.h"
#ifdef ESPUI
#include "html/HTML_PAGE.h"      // Added HTML Index Page
#include "html/CSS_STYLE.h"      // Added Main CSS Style
#include "html/CSS_NORMALIZE.h"  // Added Normalize CSS
#include "html/JS_JQUERY.h"

const size_t serializeSize = 512 * 20;

ADRIEsp_ui::ADRIEsp_ui(int prot)  {
}
void ADRIEsp_ui::server_initialize(ESP8266WebServer * server)  {
	_server = server;
}
void ADRIEsp_ui::socket_initialize(WebSocketsServer * socket)  {
	_webSocket = socket;
}

void ADRIEsp_ui::handleRoot(){
	bool internet = true;
	if(CDN){
		HTTPClient http;
		http.begin("http://www.google.com/");
		int httpCode = http.GET();
		if(httpCode > 0) {
			internet = true;
		}
	}
	_server->setContentLength(CONTENT_LENGTH_UNKNOWN);
	_server->send(200,"text/html", "");
	_server->sendContent(HTML_HEAD1);
	_server->sendContent(String("<title>")+ui_title+"</title>");
	_server->sendContent(HTML_HEAD2);
	_server->sendContent(String("<h4>")+ui_title+"  <span id=\"connection_status\" class=\"label\">Offline</span></h4></div><hr />");
	_server->sendContent(HTML_BODY);

	if(internet){
		_server->sendContent("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/2.2.4/jquery.min.js\"></script>");
	}
	else{
		_server->sendContent("<script src=\"/js/jquery.js\"></script>");
	}

	_server->sendContent(HTML_END);
	_server->client().stop();
}

// Generate Websockets Script for Webpage
void ADRIEsp_ui::handleSockets(){
  String ip;
  if(WiFi.localIP().toString() != "0.0.0.0"){
   ip = WiFi.localIP().toString();
  }
  else if(WiFi.softAPIP().toString() != "0.0.0.0"){
   ip = WiFi.softAPIP().toString();
  }

	String ws;
	ws = "var connection = new WebSocket(\"ws://"+ip+":81/\", ['arduino']);";
	ws += " var keys = {};";
	ws += " connection.onopen = function () {";
	ws += " $(\"#connection_status\").toggleClass(\"color-green\");";
	ws += " $(\"#connection_status\").text(\"Connected\");";
	ws += String(" connection.send(")+"\"{'mode': 'check_tb_status'}\");";
	for(int i=0; i<tb_index; i++){
	ws += String(" $(document).on('click',")+"'#tb"+i+"', function() {";
	ws += String(" if($('#tb")+i+"').hasClass(\"button\")){ console.log(\"Button Clicked!\"); connection.send("+"\"{'mode': 'tb_click', 'index': '"+i+"', 'status': 'on'}\""+"); }";
	ws += String(" else if($('#tb")+i+"').hasClass(\"button-primary\")){ connection.send("+"\"{'mode': 'tb_click', 'index': '"+i+"', 'status': 'off'}\""+"); } });";
	}
	ws += " };";
	ws += " connection.onerror = function (error) { $('#connection_status').toggleClass(\"color-red\"); $(\"#connection_status\").text(\"Error / No Connection\"); };";
	ws += " connection.onmessage = function (e) {";
	ws += " console.log(e.data); ";
	ws += " var obj = jQuery.parseJSON(e.data);";
	ws += " if(obj.mode === 'create_label'){ $('#row').append(\"<div class='two columns card'><h5>\"+obj.l_title+\"</h5><hr /><h3><span class='label'>\"+obj.l_value+\"</span></h3></div>\");}";
	ws += " else if(obj.mode === 'create_tbutton'){ $('#row').append(\"<div class='two columns card'><h5>\"+obj.tb_title+\"</h5><hr/><button id=\"+\"tb\"+obj.index+\"></button></div>\");}";
	ws += " else if(obj.mode === 't_button_startup'){ var tb_index = obj.index; for(i=0; i<tb_index; i++){ var tb_index2 = \"tb\"+i; var tb_status = obj[tb_index2]; var tb_index3 = \"#\"+tb_index2;  console.log(tb_status); if(tb_status === \"1\"){$(tb_index3).toggleClass('button-primary'); $(tb_index3).text('Turn Off');} else if(tb_status === \"0\"){$(tb_index3).toggleClass('button'); $(tb_index3).text('Turn On');} } }";
	ws += " else if(obj.mode === 't_button_click'){ var tb_index = \"tb\"+obj.index; var tb_status = obj[tb_index]; var tb_index3 = \"#\"+tb_index;  console.log(tb_status); if(tb_status == \"1\"){ console.log(\"Status Tuned ON\");  $(tb_index3).text('Turn Off'); $(tb_index3).removeClass( \"button\" ).addClass( \"button-primary\" );} else if(tb_status == \"0\"){console.log(\"Status Tuned OFF\"); $(tb_index3).text('Turn On'); $(tb_index3).removeClass( \"button-primary\" ).addClass( \"button\" ); } } };";
	_server->send(200, "application/javascript", ws);
}

// Handle Main Style CSS
void ADRIEsp_ui::handleSCSS(){
	_server->send_P(200, "text/css", CSS_STYLE);
}

// Handle Normalize CSS
void ADRIEsp_ui::handleNCSS(){
	_server->send_P(200, "text/css", CSS_NORMALIZE);
}

// Handle Jquery
void ADRIEsp_ui::handleJS(){
  	_server->send_P(200, "application/javascript", JS_JQUERY);
}

void ADRIEsp_ui::detectCDN(bool _autoState){
  CDN = _autoState;
}

void ADRIEsp_ui::title(const char* _title){
  ui_title = _title;
}

// Create Labels
void ADRIEsp_ui::label(const char* label_name,const char*  label_val){
  label_value[l_index] = label_val;
  label_title[l_index] = label_name;
  l_index++;
}

void ADRIEsp_ui::begin(){

	_server->on("/", 					std::bind(&ADRIEsp_ui::handleRoot, this));
    _server->on("/css/normalize.css", 	std::bind(&ADRIEsp_ui::handleNCSS, this));
    _server->on("/css/skeleton.css", 	std::bind(&ADRIEsp_ui::handleSCSS, this));
    _server->on("/js/jquery.js", 		std::bind(&ADRIEsp_ui::handleJS, this));
    _server->on("/js/sockets.js", 		std::bind(&ADRIEsp_ui::handleSockets, this));
}


void ADRIEsp_ui::handleWebpage(){

	for(int i=0; i<l_index; i++){
		String out;
		DynamicJsonDocument json(serializeSize);
		JsonObject root1 = json.to<JsonObject>();

		root1["mode"] = "create_label";
		root1["l_title"] = String(label_title[i]);
		root1["l_value"] = String(label_value[i]);

		serializeJson(json, out);
		_webSocket->broadcastTXT(out);
	}

	for(int i=0; i<tb_index; i++){
		String out;
		DynamicJsonDocument json(serializeSize);
		JsonObject root2 = json.to<JsonObject>();

		root2["mode"] = "create_tbutton";
		root2["index"] = String(i);
		root2["tb_title"] = tbuttontitle[i];

		serializeJson(json, out);
		_webSocket->broadcastTXT(out);
	}


}
#endif