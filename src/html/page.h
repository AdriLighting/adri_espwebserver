const char HTML_HEAD1[] PROGMEM = R"=====(
  <!DOCTYPE html>
  <html lang="en">
  <head>
  )=====";


#ifndef PAGE_H
#define PAGE_H

const char HTML_HEAD2[] PROGMEM = R"=====(
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="/css/style.css">
  </head>
  <body>
  )=====";

const char HTML_BODY[] PROGMEM = R"=====(
    <div class="sidenav">
      <div class="sidenavitem">
        <a onclick="request_home()" href="#">HOME</a>
         <a onclick="request_tools()" href="#TOOLS">TOOLS</a>
        <a onclick="request_wifi()" href="#">WIFI</a>
        <a onclick="request_system()" href="#">SYSTEM</a>
        <a onclick="request_api()" href="#">API</a>
        </br>
        <div id="region_menu_label">
        </div>  
        <div style="clear: both;">
          </br>
          <label class="little" onclick="debug_show()">[D]</label>
          <label class="little" >[R]</label>
        </div>
      </div>
    </div>
    <div class="content ">
      <div id="region_debug" class="" style="display:none">
        <form class="form-inline"><label>test</label><input id="inp_test" class="input-text inline color-grey" type="text"></form>
        <button class="button rounded" onclick="debug_element_createAll()">add</button>
        <button class="button rounded" onclick="debug_element_byId();">test</button>
        <button class="button rounded" onclick="insertAdjacentHTML_removeById();">remove</button>
        <button class="button rounded" onclick="debug_requestSend();">socket</button>
        <button class="button rounded" onclick="debug_toggle();">toglle</button>
      </div>    
      <div id="reg_h" class="" style="display:">
      </div>    
      <div id="reg_s" class="" style="display:">
      </div>        
    </div>
  )=====";

const char HTML_END[] PROGMEM = R"=====(
  <script src="/js/jquery-3.4.1.min.js"></script>
  <script src="/js/main.js"></script>
  </body>
  </html>

)=====";


#endif // PAGE_H

