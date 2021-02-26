#include "esp_ui_item.h"
#ifdef ESPUI

#include <adri_tools_v2.h>
// #include "factory.h"


// region ################################################ ELEMENT
void elementToString(mEspUI_elments mod, String & result){
	result = "";
	switch (mod) {
	    case meEspUI_sidebarLblValue:
	    	result = "sidebarLblValue";
	    break;
	    case meEspUI_button:
	    	result = "button";
	    break;
	    default:
	    	break;
	}
}

espUI_item * espUI_itemArray[10];
int espUI_itemArrayPos = 0;

espUI_item::espUI_item(mEspUI_elments mod, int pos, String name){

	_element = mod;
	// elementToString(mod, _elementStr);

	switch (mod) {
	    case meEspUI_sidebarLblValue:
	    	_name_1 	= name;
	    	_id_1 		= "sidenav-item-text-" + String(pos);
	    	_id_2 		= "sidenav-item-value-" + String(pos);
	    break;
	    case meEspUI_card:
	    	_name_1 	= name;
	    	_id_1 		= "card-" + String(pos);
	    break;
	    case meEspUI_input_text:
	    	_name_1 	= name;
	    	_id_1 		= "input-text-" + String(pos);
	    	_id_2 		= "button-" + String(pos);
	    break;	
	    case meEspUI_button:
	    	_name_1 	= name;
	    	_id_1 		= "button-" + String(pos);
	    break;	        
	    default:

	    break;
	}
}

void espUI_item::json(JsonObject & object){
	if (_value_1_get != NULL) _value_1_get(_value_1);
	if (_value_2_get != NULL) _value_2_get(_value_2);	

	JsonObject section = object.createNestedObject(_name_1);	

	section[F("0")]	= _element;
	section[F("1")]	= _id_1;
	section[F("3")]	= _value_1;
	section[F("4")]	= _value_2;
	section[F("5")]	= _name_1;
	section[F("7")]	= _region;
}	
void espUI_item::setJson(JsonObject & section){
	if (_value_1_get != NULL) _value_1_get(_value_1);
	if (_value_2_get != NULL) _value_2_get(_value_2);	

	section[F("0")]	= _element;
	section[F("1")]	= _id_1;
	section[F("3")]	= _value_1;
	section[F("4")]	= _value_2;
	section[F("5")]	= _name_1;
	section[F("7")]	= _region;
}	
// endregion >>>> ELEMENT



// region ################################################ SYSTEM

void espUI_item::json_espInfos(JsonObject & object, int mod){
	if (_value_1_get != NULL) _value_1_get(_value_1);
	if (_value_2_get != NULL) _value_2_get(_value_2);	

	JsonObject section = object.createNestedObject(_name_1);

	setJson(section);

	adri_toolsV2 * _tools = adri_toolsv2Ptr_get();

	if(mod==0)_tools->ESP_core_info();
	if(mod==1)_tools->ESP_shortFlash_info();

	JsonObject data = section.createNestedObject("data");	

	for (int i = 0; i < _tools->tempStrArraySize; ++i) {
		if (i==0) continue;
		String s[2];
		_tools->explode(_tools->tempStrArray[i], ';', s);
		String key = s[0];
		data[key] = s[1];
    }	
}	
// endregion >>>> SYSTEM


// region ################################################ WIFI

void espUI_item::json_wifiSetting(JsonObject & object){
	JsonObject section = object.createNestedObject(_name_1);	
	setJson(section);
}	
// endregion >>>> WIFI

#endif