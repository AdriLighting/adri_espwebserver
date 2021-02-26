#ifndef ESP_UI_ITEM_H
#define ESP_UI_ITEM_H

#include "config.h"
#ifdef ESPUI
	#include <arduino.h>
	#include <ArduinoJson.h>

	#include "esp_ui_def.h"

	class espUI_item {
	private:
		mEspUI_elments 		_element;
		String _name_1 = "";
		String _name_2 = "";
	public:
		String _id_1 	= "";
		String _id_2 	= "";
		String _value_1 = "";
		String _value_2 = "";		
		String _region 	= "reg_h";	

		espUIitem_vlaue 	_value_1_get = NULL;
		espUIitem_vlaue 	_value_2_get = NULL;

		espUIitem_handle 	_handle = NULL;

		espUI_item(mEspUI_elments mod, int pos, String s);
		void json(JsonObject & object);
		void setJson(JsonObject & object);
		void json_wifiSetting(JsonObject & object);
		void json_espInfos(JsonObject & object, int mod);
	};

	extern espUI_item * espUI_itemArray[];
	extern int espUI_itemArrayPos;
#endif // ESP_UI_H


#endif // ESP_UI_ITEM_H
