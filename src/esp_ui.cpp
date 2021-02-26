#include "esp_ui.h"
#ifdef ESPUI
#include <adri_tools_v2.h>
#include <adri_wifiConnect.h>
// #include "factory.h"

#include "html/css_main.h"
#include "html/page.h"
#include "html/js_core.h"
#include "html/js_query.h"


namespace {
	size_t json_serializeSize = 512 * 2;

	void sidebar_onTime(String & result){
		adri_toolsV2 * ptr = adri_toolsv2Ptr_get();
		result = ptr->on_time();
	}	
	void sidebar_freeHeap(String & result){
		result = String(ESP.getFreeHeap());
	}

}

using namespace std::placeholders;



// region ################################################ ESPUI
espUI * espUI_ptr = nullptr;
espUI * espUI_ptrGet(){
	return espUI_ptr;
}

espUI::espUI(ESP8266WebServer * server, WebSocketsServer * socket){
	espUI_ptr = this;
	_server = server;
	_webSocket = socket;

	_sidebarOnTime = new espUI_item(meEspUI_sidebarLblValue, _element_pos, "OnTime");
	_sidebarOnTime->_value_1_get = sidebar_onTime;
	_element_pos++;
	_sidebarFreeHeap = new espUI_item(meEspUI_sidebarLblValue, _element_pos, "FreeHeap");
	_sidebarFreeHeap->_value_1_get = sidebar_freeHeap;
	_element_pos++;
	_cardEspCoreInfos = new espUI_item(meEspUI_card, _element_pos, "Esp core infos");
	_element_pos++;
	_cardEspFlashnfos = new espUI_item(meEspUI_card, _element_pos, "Esp flash infos");
	_element_pos++;
	_button_espReset = new espUI_item(meEspUI_button, _element_pos, "reset esp");
	_element_pos++;

	card_create_espInfos();
	sidebar_create_label();
	
	wifiSetting();
	inputText_create_wifiSetting();
	
	_timer_upd 		= new adri_timer(5000, "", true);
	_timer_upd_2 	= new adri_timer(2200, "", false);
}
espUI_item * espUI::espUI_createItem(mEspUI_elments mod, String s) {
	espUI_item * myItem = new espUI_item(mod, _element_pos, s);
	_element_pos++;
	return myItem;	
}
void espUI::wifiSetting(){
	adri_toolsV2 * _tools = adri_toolsv2Ptr_get();
	for (int i = 0; i < wifiSettingList_count; ++i)
	{
		String id = _tools->ch_toString(wifiSettingList[i].id);
		_wifiSetting[i] = new espUI_item(meEspUI_input_text, _element_pos, id);
		_element_pos++;
	}	
}

void espUI::loop(){
	if (_timer_upd->loop()) {
		if (_upd_mod == 0) {
			sidebar_update_label();
			if (_userLoop!=NULL) {
				if (!_timer_upd_2->isActivate()) _timer_upd_2->activate();
			}
		}
		if (_upd_mod == 1) {card_create_espInfos(); 		_upd_mod = 0;}
		if (_upd_mod == 2) {inputText_create_wifiSetting(); _upd_mod = 0;}
	}
	if ((_upd_mod == 0) && (_timer_upd_2->isActivate())) {
		if (_timer_upd_2->loop()) {
			if (_userLoop!=NULL) _userLoop();
		}
	}
}	
// endregion >>>> ESPUI


// region ################################################ INITIALISE WEBSERVER

// void adri_webserver::handleGetEdit() {
// 	if (handleFileRead(F("/edit/index.htm"))) {
// 		return;
// 	}

// #ifdef INCLUDE_FALLBACK_INDEX_HTM
// 	_server.sendHeader(F("Content-Encoding"), "gzip");
// 	_server.send(200, "text/html", index_htm_gz, index_htm_gz_len);
// #else
// 	replyNotFound(FPSTR(FILE_NOT_FOUND));
// #endif
// }

void espUI::handleSCSS(){
	_server->send_P(200, "text/css", CSS_STYLE);
}
void espUI::handleJS_CORE(){
	_server->send_P(200, "application/javascript", JS_CORE);
}	
void espUI::handleJS(){
 	 _server->send_P(200, "application/javascript", JS_JQUERY);
}

void espUI::handleRoot(){
	bool internet = false;
	// if(CDN){
	// 	HTTPClient http;
	// 	http.begin("http://www.google.com/");
	// 	int httpCode = http.GET();
	// 	if(httpCode > 0) {
	// 		internet = true;
	// 	}
	// }
	_server->setContentLength(CONTENT_LENGTH_UNKNOWN);
	_server->send(200,"text/html", "");
	_server->sendContent(HTML_HEAD1);
	_server->sendContent(String("<title>")+"ui_title"+"</title>");
	_server->sendContent(HTML_HEAD2);
	// _server->sendContent(String("<h4>")+ui_title+"  <span id=\"connection_status\" class=\"label\">Offline</span></h4></div><hr />");
	_server->sendContent(HTML_BODY);

	// if(internet){
	// _server->sendContent("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/2.2.4/jquery.min.js\"></script>");
	// }
	// else{
	// _server->sendContent("<script src=\"/js/jquery.js\"></script>");
	// }

	_server->sendContent(HTML_END);
	_server->client().stop();
}	

/*
void espUI::handleSCSS(){

}
void espUI::handleJS_CORE(){

}	
void espUI::handleJS(){
	// if (handleFileRead(F("/jquery-3.4.1.min.js"))) {
	// 	return;
	// }
}

void espUI::handleRoot(){
	// if (handleFileRead(F("/index.htm"))) {
	// 	return;
	// }
}	
*/

void espUI::handleRootRegister(){
	_server->on("/", 		std::bind(&espUI::handleRoot, this));
}		
void espUI::handleAssetRegister(){
	_server->on("/css/style.css", 			HTTP_GET, std::bind(&espUI::handleSCSS, this));
	_server->on("/js/jquery-3.4.1.min.js", 	HTTP_GET, std::bind(&espUI::handleJS, this));
	_server->on("/js/main.js", 				HTTP_GET, std::bind(&espUI::handleJS_CORE, this));
}		
// endregion >>>> INITIALISE WEBSERVER



// region ################################################ SIDEBAR LBL
void espUI::sidebar_create_label(){
	DynamicJsonDocument json(json_serializeSize);
	JsonObject root = json.to<JsonObject>();	

	JsonObject object = root.createNestedObject("create");	

	_sidebarOnTime->json(object);
	_sidebarFreeHeap->json(object);

	String result;
	serializeJson(json, result);
	_webSocket->sendTXT(0, result);
}
void espUI::sidebar_update_label(){
	DynamicJsonDocument json(json_serializeSize);
	JsonObject root = json.to<JsonObject>();	
	JsonObject object = root.createNestedObject("upd");	
	_sidebarOnTime->json(object);
	_sidebarFreeHeap->json(object);
	String result;
	serializeJson(json, result);
	_webSocket->sendTXT(0, result);
}	
// endregion >>>> SIDEBAR LBL

// region ################################################ USER CREATE
// espUIitem_handle _tesftfunc = NULL;
// void _testfuncwparm(espUIitem_handle v){
// 	_tesftfunc = v;
// }
void espUI::user_button_loop(String parse){
	String result;
	boolean find = false; 	
	for(int i = 0; i < espUI_itemArrayPos; ++i) {
		find = commandSplit(parse, espUI_itemArray[i]->_id_1, result); 	
		if (find) {
			espUI_itemArray[i]->_handle(result);
		}
	}
}
int espUI::user_button_setup(String name, String btName, espUIitem_handle f){

	int pos = espUI_itemArrayPos;

	espUI_itemArray[pos] = new espUI_item(meEspUI_button, _element_pos, name);

	_element_pos++;
	espUI_itemArrayPos++;

	espUI_itemArray[pos]->_region = "reg_s";
	espUI_itemArray[pos]->_value_1 = btName;
	espUI_itemArray[pos]->_handle = f;

	return pos;
}	
void espUI::user_button_create(int pos){
	DynamicJsonDocument json(json_serializeSize);
	JsonObject root = json.to<JsonObject>();	

	JsonObject object = root.createNestedObject("create");	

	espUI_itemArray[pos]->json(object);

	String result;
	serializeJson(json, result);
	_webSocket->sendTXT(0, result);
}
// endregion >>>> USER CREATE

// region ################################################ HOME / TOOLS / API
void espUI::button_create_espRest(){
	DynamicJsonDocument json(json_serializeSize);
	JsonObject root = json.to<JsonObject>();	

	JsonObject object = root.createNestedObject("create");	

	_button_espReset->json(object);

	String result;
	serializeJson(json, result);
	_webSocket->sendTXT(0, result);
}
void espUI::button_handle_espRest(String command){
	String result;
	boolean find = commandSplit(command, _button_espReset->_id_1, result); 
	if (find) {
		ESP.restart();
	}
}
void espUI::tools_create(){
	button_create_espRest();
}
void espUI::removeElements_tools(){
	DynamicJsonDocument json(json_serializeSize);
	JsonObject root = json.to<JsonObject>();	

	JsonObject object = root.createNestedObject("remove");	

	String id_1;

	id_1 = _button_espReset->_id_1;
	if(id_1 != "") object[id_1] = id_1;

	String result;
	serializeJson(json, result);
	_webSocket->sendTXT(0, result);	
}	
void espUI::home_create(){
}
void espUI::removeElements_home(){
}	
void espUI::api_create(){
}
void espUI::removeElements_api(){
}
// endregion >>>> HOME


// region ################################################ SYSTEM
void espUI::removeElements_system(){
	DynamicJsonDocument json(json_serializeSize);
	JsonObject root = json.to<JsonObject>();	

	JsonObject object = root.createNestedObject("remove");	

	String id_1, id_2;

	id_1 = _cardEspCoreInfos->_id_1;
	id_2 = _cardEspCoreInfos->_id_2;
	if(id_1 != "") object[id_1] = id_1;
	if(id_2 != "") object[id_2] = id_2;

	id_1 = _cardEspFlashnfos->_id_1;
	id_2 = _cardEspFlashnfos->_id_2;
	if(id_1 != "") object[id_1] = id_1;
	if(id_2 != "") object[id_2] = id_2;

	String result;
	serializeJson(json, result);
	_webSocket->sendTXT(0, result);
}
void espUI::card_create_espInfos(){
    File file = LittleFS.open("/json/espInfos.json", "w");
    if (file) {	
		DynamicJsonDocument json(json_serializeSize);
		JsonObject root = json.to<JsonObject>();	
		JsonObject object = root.createNestedObject("create");	
		_cardEspCoreInfos->json_espInfos(object, 0);
		_cardEspFlashnfos->json_espInfos(object, 1);
		serializeJson(json, file);
	}
}
// endregion >>>> SYSTEM


// region ################################################ WIFI
void espUI::removeElements_wifiSetting(){
	DynamicJsonDocument json(json_serializeSize);
	JsonObject root = json.to<JsonObject>();	

	JsonObject object = root.createNestedObject("remove");	

	String id_1;

	for (int i = 0; i < wifiSettingList_count; ++i) {
		id_1 = _wifiSetting[i]->_id_1;
		if(id_1 != "") object[id_1] = id_1;
	}

	String result;
	serializeJson(json, result);
	_webSocket->sendTXT(0, result);
}
void espUI::inputText_create_wifiSetting(){
    File file = LittleFS.open("/json/wifisetting.json", "w");
    if (file) {	
		DynamicJsonDocument json(512*4);
		JsonObject root = json.to<JsonObject>();	
		JsonObject object = root.createNestedObject("create");	
		adri_toolsV2 * _tools = adri_toolsv2Ptr_get();
		for (int i = 0; i < wifiSettingList_count; ++i)
		{
			_wifiSetting[i]->_value_1 = _tools->ch_toString(wifiSettingList[i].id);
			wifiSettingList[i].getValue(_wifiSetting[i]->_value_2);
			_wifiSetting[i]->json_wifiSetting(object);
		}

		serializeJson(json, file);
	}
}
// endregion >>>> WIFI



boolean espUI::commandSplit(String command, String value, String & result){
	String  s_command;
	String  s_valueCommand;
	String  s_readString = command;
	char    inputChar[s_readString.length() + 1] ;
			s_readString.toCharArray(inputChar, s_readString.length() + 1);
	char    * Rcommand  = strtok(inputChar, "&");

	boolean find = false;
	result = "";

	while (Rcommand != 0){          
		char* separator  = strchr(Rcommand, '=');
		if (separator != 0) {
			*separator  = 0;            
			++separator ;
			s_command = String(Rcommand);
			s_valueCommand = String(separator); 
			if (value == s_command) {
				result = s_valueCommand;
				find = true;
				break;
			}

		}
		Rcommand = strtok(0, "&"); 
	}	
	return find;

}

void espUI::removeElements(){
	if (_region == 0) removeElements_home();
	if (_region == 1) removeElements_system();
	if (_region == 2) removeElements_wifiSetting();
	if (_region == 3) removeElements_tools();
	if (_region == 4) removeElements_api();	
}
#endif