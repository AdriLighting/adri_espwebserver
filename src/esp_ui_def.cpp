#include "esp_ui_def.h"

#ifdef ESPUI

#include <adri_wifiConnect.h>

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

// region ################################################ WIFISETTING LIST
void wifiSetting_get_wifi_dnsname(String & result) 	{ result = wifiConnect_instance()->dnsName_get();		}
void wifiSetting_get_wifi_hostname(String & result)	{ result = wifiConnect_instance()->hostName_get();		}
void wifiSetting_get_wifi_reset(String & result)   	{ result = wifiConnect_instance()->connect_get();		}
void wifiSetting_get_wifi_subnet(String & result)  	{ result = WiFi.subnetMask().toString();				}
void wifiSetting_get_wifi_gateway(String & result) 	{ result = WiFi.gatewayIP().toString();					}
void wifiSetting_get_wifi_ip(String & result )		{ result = wifiConnect_instance()->currentIp_get();		}
void wifiSetting_get_wifi_ssid(String & result)    	{ result = wifiConnect_instance()->staSsid_get();		}
void wifiSetting_get_wifi_psk(String & result)     	{ result = wifiConnect_instance()->staPsk_get();		}
void wifiSetting_get_wifiAp_ip(String & result)    	{ result = wifi_credentialAp_ptr_get()->ip_get();		}
void wifiSetting_get_wifiAp_psk(String & result)   	{ result = wifi_credentialAp_ptr_get()->psk_get();		}
void wifiSetting_get_wifiAp_ssid(String & result)  	{ result = wifi_credentialAp_ptr_get()->hostname_get();	}
const char PROGMEM rep_wifi_dnsname 	[] = "wifi_dnsname";
const char PROGMEM rep_wifi_hostname 	[] = "wifi_hostname";
const char PROGMEM rep_wifi_reset 		[] = "wifi_reset";
const char PROGMEM rep_wifi_subnet 		[] = "wifi_subnet";
const char PROGMEM rep_wifi_gateway 	[] = "wifi_gateway";
const char PROGMEM rep_wifi_ip 			[] = "wifi_ip";
const char PROGMEM rep_wifi_ssid 		[] = "wifi_ssid";
const char PROGMEM rep_wifi_psk 		[] = "wifi_psk";
const char PROGMEM rep_wifiAp_ip		[] = "wifiAp_ip";
const char PROGMEM rep_wifiAp_psk 		[] = "wifiAp_psk";
const char PROGMEM rep_wifiAp_ssid 		[] = "wifiAp_ssid";
PROGMEM espUI_itemList wifiSettingList [] = {
	{rep_wifi_reset,	&wifiSetting_get_wifi_reset 		},
	{rep_wifi_dnsname,	&wifiSetting_get_wifi_dnsname		},
	{rep_wifi_hostname,	&wifiSetting_get_wifi_hostname		},
	{rep_wifi_subnet,   &wifiSetting_get_wifi_subnet		},
	{rep_wifi_gateway,	&wifiSetting_get_wifi_gateway		},	
	{rep_wifi_ip,       &wifiSetting_get_wifi_ip    		},
	{rep_wifi_ssid,     &wifiSetting_get_wifi_ssid  		},
	{rep_wifi_psk,      &wifiSetting_get_wifi_psk   		},
	// {rep_wifiAp_ip,     &wifiSetting_get_wifiAp_ip  		},
	{rep_wifiAp_psk,    &wifiSetting_get_wifiAp_psk 		},
	// {rep_wifiAp_ssid,   &wifiSetting_get_wifiAp_ssid,		}
};
int wifiSettingList_count = ARRAY_SIZE(wifiSettingList);	


#endif
