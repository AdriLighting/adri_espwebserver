#ifndef ESP_UI_DEF_H
	#define ESP_UI_DEF_H

	#include "config.h"
#ifdef ESPUI	

	#include <arduino.h>


	struct espUI_itemList {
		const char * 					id;
		void (* getValue	) 			(String & result);
	} ;	

	enum mEspUI_elments
	{
		meEspUI_button,
		meEspUI_sidebarLblValue,
		meEspUI_input_range,
		meEspUI_card,
		meEspUI_input_text
	};


	typedef void (*espUIitem_handle)(String result);  
	typedef void (*espUIitem_vlaue)(String & result);
	typedef void (*espUI_userLoop)(); 
	
	extern PROGMEM espUI_itemList wifiSettingList [] ;
	extern int wifiSettingList_count;
#endif // ESP_UI_DEF_H
#endif // ESP_UI_DEF_H
		

