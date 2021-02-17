# adri_espwebserver

- Librairie pour compléter ESP826WebServer.
- Facilite la configuration du serveur et des requête.
- Automatise le traitement des requêtes.

## Quickstart
Static WebSockets and Webserver object 
```c++
adri_webserver      clientServer(80); // instance adri_webserver    - ESP8266WebServer _server
adri_socket         socketServer(81); // instance adri_socket       - WebSocketsServer _socket
```
setup
```c++
clientServer.filesystem_ok(true);       // ENABLED FILE SYSTEM BROWSER
clientServer.filesystem_set(&LittleFS);
clientServer.initialize(80);
clientServer.begin();

socketServer.setup();
```
loop
```c++
clientServer.handleLoop();
socketServer.loop();
```
see https://github.com/AdriLighting/adri_httparseurl for the request management.

<pre>
Librairies
<details>
adri_espwebserver               = https://github.com/AdriLighting/adri_espwebserver
adri_httparseurl                = https://github.com/AdriLighting/adri_httparseurl
adri_tools_v2                   = https://github.com/AdriLighting/adri_tools_v2

ArduinoJson                     = https://github.com/bblanchon/ArduinoJson
WebSockets                      = https://github.com/Links2004/arduinoWebSockets
ESPAsyncTCP                     = https://github.com/me-no-dev/ESPAsyncTCP

framework                       = https://github.com/esp8266/Arduino/tree/master/libraries
Hash                            = 
ESP8266HTTPClient               = 
ESP8266WiFi                     = 
ESP8266WebServer                = 
Ethernet(esp8266)               = 
SPI                             = 
LittleFS(esp8266)               = 
ArduinoOTA                      = 
ESP8266mDNS                     = 

</details>
Dependency Graph
<details>
|-- [adri_espwebserver] 1.0.0
|   |-- [Hash] 1.0
|   |-- [ArduinoJson] 6.17.1
|   |-- [ESP8266HTTPClient] 1.2
|   |   |-- [ESP8266WiFi] 1.0
|   |-- [ESP8266WebServer] 1.0
|   |   |-- [ESP8266WiFi] 1.0
|   |-- [ESP8266WiFi] 1.0
|   |-- [WebSockets] 2.3.5
|   |   |-- [ESP8266WiFi] 1.0
|   |   |-- [ESPAsyncTCP] 1.2.2
|   |   |   |-- [ESP8266WiFi] 1.0
|   |   |-- [Ethernet(esp8266)] 1.0.4
|   |   |   |-- [SPI] 1.0
|   |   |-- [SPI] 1.0
|   |   |-- [Hash] 1.0
|   |   |-- [ESP8266WebServer] 1.0
|   |   |   |-- [ESP8266WiFi] 1.0
|   |-- [adri_httparseurl] 1.0.0
|   |   |-- [adri_tools_v2] 1.0.0
|   |   |   |-- [ESP8266WiFi] 1.0
|   |   |   |-- [LittleFS(esp8266)] 0.1.0
|   |   |-- [ESP8266WiFi] 1.0
|   |   |-- [LittleFS(esp8266)] 0.1.0
|   |-- [ArduinoOTA] 1.0
|   |   |-- [ESP8266WiFi] 1.0
|   |   |-- [ESP8266mDNS] 1.2
|   |   |   |-- [ESP8266WiFi] 1.0
|   |-- [ESP8266mDNS] 1.2
|   |   |-- [ESP8266WiFi] 1.0
|   |-- [LittleFS(esp8266)] 0.1.0
|   |-- [adri_tools_v2] 1.0.0
|   |   |-- [ESP8266WiFi] 1.0
|   |   |-- [LittleFS(esp8266)] 0.1.0</details>
</pre>

<hr>

#### examples

adri_espwebserver
<details>
<pre>
platformio.ini
<details>
[env:nodemcuv2]
platform=espressif8266
board=nodemcuv2
framework=arduino
lib_ignore=WiFi101
board_build.filesystem=littlefs
board_build.ldscript=eagle.flash.4m3m.ld
lib_extra_dirs= ${env.lib_extra_dirs}
upload_speed=921600
[platformio]
src_dir= ${env.src_dir}

</details>
</pre>
<pre>
Librairies
<details>
adri_espwebserver               = https://github.com/AdriLighting/adri_espwebserver
adri_wifiConnect                = https://github.com/AdriLighting/adri_wifiConnect
adri_tools_v2                   = https://github.com/AdriLighting/adri_tools_v2
adri_timer                      = https://github.com/AdriLighting/adri_timer
adri_httparseurl                = https://github.com/AdriLighting/adri_httparseurl

ArduinoJson                     = https://github.com/bblanchon/ArduinoJson
WebSockets                      = https://github.com/Links2004/arduinoWebSockets
ESPAsyncTCP                     = https://github.com/me-no-dev/ESPAsyncTCP

framework                       = https://github.com/esp8266/Arduino/tree/master/libraries
ESP8266WebServer                = 
ESP8266WiFi                     = 
ESP8266mDNS                     = 
LittleFS(esp8266)               = 
Hash                            = 
ESP8266HTTPClient               = 
Ethernet(esp8266)               = 
SPI                             = 
ArduinoOTA                      = 

</details>
Dependency Graph
<details>
|-- [adri_espwebserver] 1.0.0
|   |-- [adri_wifiConnect] 1.0.0
|   |   |-- [ESP8266WebServer] 1.0
|   |   |   |-- [ESP8266WiFi] 1.0
|   |   |-- [ESP8266WiFi] 1.0
|   |   |-- [ESP8266mDNS] 1.2
|   |   |   |-- [ESP8266WiFi] 1.0
|   |   |-- [adri_tools_v2] 1.0.0
|   |   |   |-- [ESP8266WiFi] 1.0
|   |   |   |-- [LittleFS(esp8266)] 0.1.0
|   |   |-- [LittleFS(esp8266)] 0.1.0
|   |   |-- [adri_timer] 1.0.0
|   |-- [LittleFS(esp8266)] 0.1.0
|   |-- [Hash] 1.0
|   |-- [ArduinoJson] 6.17.1
|   |-- [ESP8266HTTPClient] 1.2
|   |   |-- [ESP8266WiFi] 1.0
|   |-- [ESP8266WebServer] 1.0
|   |   |-- [ESP8266WiFi] 1.0
|   |-- [ESP8266WiFi] 1.0
|   |-- [WebSockets] 2.3.5
|   |   |-- [ESP8266WiFi] 1.0
|   |   |-- [ESPAsyncTCP] 1.2.2
|   |   |   |-- [ESP8266WiFi] 1.0
|   |   |-- [Ethernet(esp8266)] 1.0.4
|   |   |   |-- [SPI] 1.0
|   |   |-- [SPI] 1.0
|   |   |-- [Hash] 1.0
|   |   |-- [ESP8266WebServer] 1.0
|   |   |   |-- [ESP8266WiFi] 1.0
|   |-- [adri_httparseurl] 1.0.0
|   |   |-- [adri_tools_v2] 1.0.0
|   |   |   |-- [ESP8266WiFi] 1.0
|   |   |   |-- [LittleFS(esp8266)] 0.1.0
|   |   |-- [ESP8266WiFi] 1.0
|   |   |-- [LittleFS(esp8266)] 0.1.0
|   |-- [ArduinoOTA] 1.0
|   |   |-- [ESP8266WiFi] 1.0
|   |   |-- [ESP8266mDNS] 1.2
|   |   |   |-- [ESP8266WiFi] 1.0
|   |-- [ESP8266mDNS] 1.2
|   |   |-- [ESP8266WiFi] 1.0
|   |-- [adri_tools_v2] 1.0.0
|   |   |-- [ESP8266WiFi] 1.0
|   |   |-- [LittleFS(esp8266)] 0.1.0
|-- [adri_timer] 1.0.0
|-- [adri_tools_v2] 1.0.0
|   |-- [ESP8266WiFi] 1.0
|   |-- [LittleFS(esp8266)] 0.1.0
|-- [adri_wifiConnect] 1.0.0
|   |-- [ESP8266WebServer] 1.0
|   |   |-- [ESP8266WiFi] 1.0
|   |-- [ESP8266WiFi] 1.0
|   |-- [ESP8266mDNS] 1.2
|   |   |-- [ESP8266WiFi] 1.0
|   |-- [adri_tools_v2] 1.0.0
|   |   |-- [ESP8266WiFi] 1.0
|   |   |-- [LittleFS(esp8266)] 0.1.0
|   |-- [LittleFS(esp8266)] 0.1.0
|   |-- [adri_timer] 1.0.0
|-- [LittleFS(esp8266)] 0.1.0</details>
</pre>

</details>
<hr>