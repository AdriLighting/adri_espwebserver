#ifndef JS_CORE_H
#define JS_CORE_H

const char JS_CORE[] PROGMEM = R"=====(
// 	###################################################################################### VARIABLES
// 	SOCKET VARIABLES 
var socket_connect_display    = true;
var socket_display            = true;
var web_socket                = null;
var socket_connected          = false;
var socket_keep_alive_period  = 500;
var socket_keep_alive_enabled = false;
var response_display          = true;


// 	###################################################################################### ["4","prise",1,1,1,2,14,"_id"]


// 	###################################################################################### FUNCTIONS
// 	
// 	###########################################
// 	SOCKET FUNCTIONS 
// 	
function socket_initialize(){
	console.log("\n[socket_initialize]\n");
	// web_socket = new WebSocket('ws://'+document.location.host+'/ws',['arduino']);
	web_socket = new WebSocket('ws://' + window.location.hostname + ':81/');
	// web_socket = new ReconnectingWebSocket('ws://' + window.location.hostname + ':81/', ['arduino']);
	web_socket.binaryType = "arraybuffer";
	web_socket.debug = true;
	console.log('ws://'+document.location.host+'/ws\n');

	web_socket.onopen = function(e) { 
		if (socket_connect_display) console.log("\n[socket onopen]\n\t", e);
		socket_connected=true;
		socket_status_display();
		if (socket_keep_alive_enabled) keep_alive_time_out=setTimeout(socket_keep_alive, socket_keep_alive_period);
	};
	 
	web_socket.onclose = function(e){
		if (socket_connect_display) console.log("\n[socket onclose]\n\t", e);
		socket_connected=false;
		socket_status_display();
		// setTimeout(function() {  socket_init()  }, 1000);
	};
	 
	web_socket.onerror = function(e){
		if (socket_connect_display) console.log("\n[socket onerror]\n\t", e);
		socket_connected=false;
		// setTimeout(function() {  socket_init()  }, 1000);
		socket_status_display();
	};
	 
	web_socket.onmessage = function(e){
		var msg = "";
		if (e.data instanceof ArrayBuffer){
			msg = "BIN:";
			var bytes = new Uint8Array(e.data);
			for (var i = 0; i < bytes.length; i++) {
				msg += String.fromCharCode(bytes[i]);
			}
		} else {
			msg = e.data;
		}
		socket_receive(msg);
	};
}

function socket_keep_alive() {
	if (!web_socket) return;
	if (web_socket.readyState !== 1) return;
	// client_request("socket_keep_alive");
	keep_alive_time_out=setTimeout(socket_keep_alive, socket_keep_alive_period);
}

function socket_receive(msg) {
	if (socket_display) console.log("\n[socket_receive]\n\t"+msg+"\n");
	server_response(msg);
}

function socket_send(msg) {
	if (! socket_connected ) socket_initialize();
	web_socket.send(msg); 
	if (socket_display) console.log("\n[socket_send]\n\t"+msg+"\n");
}

function socket_status_display() {
	// if (socket_connected) document.getElementById('lbl_upd').value = "connected";
	// else document.getElementById('lbl_upd').value = "disconnected";
}


// 	###########################################
// 	JSON FUNCTIONS 
// 	
function hasJsonStructure(str) {
    if (typeof str !== 'string') return false;
    try {
        const result = JSON.parse(str);
        const type = Object.prototype.toString.call(result);
        return type === '[object Object]' 
            || type === '[object Array]';
    } catch (err) {
        return false;
    }
}
function safeJsonParse(str) {
    try {
        return [null, JSON.parse(str)];
    } catch (err) {
        return [err];
    }
}

function printValues(obj) {
    for(var [k, v] in obj) {
        if(obj[k] instanceof Object) {
            printValues(obj[k] );

        } else {
            document.write(obj[k] + "<br>");

        };
    }
};
// printValues(response) ;
function visit(object) {
    if (isIterable(object)) {
        forEachIn(object, function (accessor, child) {
            visit(child);
        });
    }
    else {
        var value = object;
        console.log(value);
    }
}

function forEachIn(iterable, functionRef) {
    for (var accessor in iterable) {
        functionRef(accessor, iterable[accessor]);
    }
}

function isIterable(element) {
    return isArray(element) || isObject(element);
}

function isArray(element) {
    return element.constructor == Array;
}

function isObject(element) {
    return element.constructor == Object;
}

function isUndefined(element) {
    return element == undefined;
}

// 	###########################################
// 	ARRAY FUNCTIONS
// 	
function getDim(a) {
    var dim = [];
    for (;;) {
        dim.push(a.length);

        if (Array.isArray(a[0])) {
            a = a[0];
        } else {
            break;
        }
    }
    return dim;
}

// 	###########################################
// 	REQUEST FUNCTIONS
// 	
function literal_item(name, value) {
   var x="<"+name+">"+value+"</"+name+">";
   return x;
}

function request_format_http(name, arg, value){
	var ret = "/" + name + "?&" + arg + "=" + value;
	return ret;
}
function request_format_socket(name, arg, value){
	var cmd = "&" + arg + "=" + value;
	var op 	= literal_item("op", name);
	var msg = literal_item("cmd", cmd);
	return op + msg;
}

function request_send(name, arg, value){
	// console.log(" window.location.hostname");
//	request_http("/json/currentSetting.json");
	var msg="";
	if (socket_connected) {
		msg = request_format_socket(name, arg, value);
		socket_send(msg);
	} else {
		msg = request_format_http(name, arg, value);
		request_http(msg);
	}
}
function request_send_http(name, arg, value){
	var msg = request_format_http(name, arg, value);
	request_http(msg);
}
function request_send_socket(name, arg, value){
	var msg = request_format_socket(name, arg, value);
	socket_send(msg);
}
function request_http(msg){
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
	    if (this.readyState == 4 && this.status == 200) {
	       // Typical action to be performed when the document is ready:
	       server_response(xhttp.responseText);
	    }
	};
	xhttp.open("GET", msg, true);
	xhttp.send();	
}
function request_button(id){

}
function request_system(){
	document.getElementById("reg_s").style.display = "none";
	var op 	= literal_item("op", "espUI");
	var msg = literal_item("cmd", "espUI_system");
	socket_send(op+msg);
	request_http("/json/espInfos.json");
}
function request_wifi(){
	document.getElementById("reg_s").style.display = "none";
	var op 	= literal_item("op", "espUI");
	var msg = literal_item("cmd", "espUI_wifi");
	socket_send(op+msg);
	request_http("/json/wifisetting.json");
}
function request_home(){
	document.getElementById("reg_s").style.display = "";
	var op 	= literal_item("op", "espUI");
	var msg = literal_item("cmd", "espUI_main");
	socket_send(op+msg);
}
function request_tools(){
	document.getElementById("reg_s").style.display = "none";
	var op 	= literal_item("op", "espUI");
	var msg = literal_item("cmd", "espUI_tools");
	socket_send(op+msg);
}
function request_api(){
	document.getElementById("reg_s").style.display = "none";
	var op 	= literal_item("op", "espUI");
	var msg = literal_item("cmd", "espUI_api");
	socket_send(op+msg);
}
// 	#############################################################################################
// 	######################################################################################
// 								PARSING SOCKET REQUEST RECIEVE 
								
// 	SOCKET RECEIVE DATA 
function server_response(json) {
	if (json == "{\"}") return;

	if (response_display) { console.log("\n[server_response]\n\t string:\n"+json+"\n"); }

	const [err, result] = safeJsonParse(json);
	if (err) {
		if (response_display) console.log('\tFailed to parse JSON: ' + err.message);
		return;
	}
	
	var response = JSON.parse(json);

	if (response_display) {console.log('\tjson:\n');console.log(response);console.log("\n");}

	sidebarLblValue_create(response, "");
	sidebarLblValue_update(response, "");

	removeElements(response, "");
	card_create_v2(response, 3, "");

	inputText_create(response, "");
	button_create(response, "");
}

function jsonArrayDebug(val) {
	for (const okey of  Object.entries(val)) {
		console.log("\t" + "\t" + okey[0]  + " : " + okey[1]);
	}
}
// function insertAdjacentHTML_button (region = 'reg_h', name = lem_test, id = 'button-'+lem_test) {
function button_create(json, debug){

	var jsonObject = json["create"];
	if (jsonObject == undefined) return;

	for (const key of Object.keys(jsonObject)) {

		if (debug) console.log(key);
		if (jsonObject[key]["0"] != 0) continue;

		insertAdjacentHTML_button(jsonObject[key]["7"], jsonObject[key]["5"], jsonObject[key]["1"]);
		document.getElementById(jsonObject[key]["1"]).text  = jsonObject[key]["3"];
	}
}
        // function insertAdjacentHTML_inpuText (region = 'reg_h', name = lem_test, id_inp = 'input-text-'+lem_test, id_button = 'input-text-button-'+lem_test) {
function inputText_create(json, debug){

	var jsonObject = json["create"];
	if (jsonObject == undefined) return;

	for (const key of Object.keys(jsonObject)) {

		if (debug) console.log(key);
		if (jsonObject[key]["0"] != 4) continue;

		insertAdjacentHTML_inpuText(jsonObject[key]["7"], jsonObject[key]["3"], jsonObject[key]["1"]);
		document.getElementById(jsonObject[key]["1"]).value = jsonObject[key]["4"];
		document.getElementById(jsonObject[key]["1"] + "-button").innerHTML = "send";
	}
}
function sidebarLblValue_create(json, debug){

	var jsonObject = json["create"];
	if (jsonObject == undefined) return;

	for (const key of Object.keys(jsonObject)) {

		if (debug) console.log(key);
		if (jsonObject[key]["0"] != 1) continue;

		insertAdjacentHTML_sidenavItemTxt("region_menu_label", jsonObject[key]["5"], jsonObject[key]["1"] );
		document.getElementById(jsonObject[key]["1"] + "-data").innerHTML = jsonObject[key]["3"];
	}
}
function sidebarLblValue_update(json, debug){
	var jsonObject = json["upd"];
	if (jsonObject == undefined) return;
	for (const key of Object.keys(jsonObject)) {
		if (jsonObject[key]["0"] != 1) continue;
		if (document.getElementById(jsonObject[key]["1"] + "-data") != null) document.getElementById(jsonObject[key]["1"] + "-data").innerHTML = jsonObject[key]["3"];
	}	
}

function removeElements(json, debug){

	var jsonObject = json["remove"];
	if (jsonObject == undefined) return;

	for (const key of Object.keys(jsonObject)) {
		if (isObject(jsonObject[key])) {
		}else {
			if (debug) console.log(key + " : " + jsonObject[key]);
			insertAdjacentHTML_removeById(key);
		}
	}
}
			 // 
function card_jsonArrayParse(val) {
	var content = "";
	for (const okey of  Object.entries(val)) {
		content += '<label class="lblValue">'+okey[0]+'</label><span class="lblValue">'+okey[1]+'</span></br>' + '\n';
	}
	return content;
}
function card_create_v2(json, type, debug){

	var jsonObject = json["create"];
	if (jsonObject == undefined) return;
	var data = "";
	for (const key of Object.keys(jsonObject)) {

		if (jsonObject[key]["0"] != type) continue;

		data = "";
		if (isObject(jsonObject[key])) {
			for (const okey of  Object.entries(jsonObject[key])) {
				if (isObject(okey[1])) {
					data = card_jsonArrayParse(okey[1]);
				}
			}
			if (data!="") {
			    document.querySelector('#'+jsonObject[key]["7"]).insertAdjacentHTML(
			        'beforeend',`
				      <div class="container">
				        <div id="reg-`+jsonObject[key]["7"]+`" class="row">

				        </div>
				      </div>       
			    ` 
			   	);
			    document.querySelector('#reg-'+jsonObject[key]["7"]).insertAdjacentHTML(
			        'beforeend',`
				          <div id=`+jsonObject[key]["1"]+` class="column">
				            <div class="card" style="width:100%;">
				              <header class="container color-grey">
				                <label id=`+jsonObject[key]["1"]+`-lbl class="test">`+jsonObject[key]["5"]+`</label>
				              </header>
				              <div id=`+jsonObject[key]["1"]+`-data class="container">
				                </br>`+data+`
				              </div>
				            </div>
				          </div>
			           `                 
			   	);
			}	
		}else {
		}
	}
}

        var lem_test = 0  
        function insertAdjacentHTML_sidenavItemTxt (region = 'region_menu_label', name = 'name'+lem_test, id = 'sidenav-item-text-'+lem_test) {
            document.querySelector('#'+region).insertAdjacentHTML(
                'beforeend',  
                '<label id='+id+' style="clear: both" class="sidenavitem textleft">'+name+'</label><label id='+id+'-data  class="sidenavitem textright">'+name+'</label>  '      
            );
            lem_test++;
        }
       

        function insertAdjacentHTML_inpuText (region = 'reg_h', name = lem_test, id = 'input-text-'+lem_test) {
            document.querySelector('#'+region).insertAdjacentHTML(
                'beforeend',
                '<div class="form-inline"><label>'+name+'</label><input id='+id+' class="input-text inline color-grey" type="text"> <button id="'+id+'-button" class="button rounded">'+name+'</button></div>   '
            )
		    document.getElementById(id+"-button").addEventListener("click", function(target) {
				var idbt = target.originalTarget.id;
				var value = document.getElementById(id).value;
				request_send("espUIRequest", idbt, value);		        
		    });             
            lem_test++;
        }

        function insertAdjacentHTML_inpuRange (region = 'reg_h', name = lem_test, id = 'input-range-'+lem_test) {
            document.querySelector('#'+region).insertAdjacentHTML(
                'beforeend',
                '<div class="form-inline"><label>'+name+'</label><input id='+id+' class="slider inline" type="range" min="1" max="100" value="50">  </div>'      
            );
            lem_test++;
		    document.getElementById(id).addEventListener("change", function(target) {
				var id = target.originalTarget.id;
				request_send("espUIRequest", id, document.getElementById(id).value);
		    });             
        }

        function insertAdjacentHTML_button (region = 'reg_h', name = lem_test, id = 'button-'+lem_test) {
            const buttons = document.querySelector('#'+region).insertAdjacentHTML(
                'beforeend',
                ' <button id='+id+' class="button rounded">'+name+'</button>'      
            )
            lem_test++;
		    document.getElementById(id).addEventListener("click", function(target) {
				var id = target.originalTarget.id;
				request_send("espUIRequest", id, id);		        
		    });            
        }

        function insertAdjacentHTML_remove (input) {
            input.parentNode.remove();
        }  
        function insertAdjacentHTML_removeById(idName = ""){
        	if ((idName == "") && (document.getElementById("inp_test") != null) ){
        		idName = document.getElementById("inp_test").value;
        	}	
			if (document.getElementById(idName) != null) {
				if (idName.indexOf("button")>=0) document.getElementById(idName).remove();
				else if (idName.indexOf("sidenav")>=0) document.getElementById(idName).remove();
				else if (idName.indexOf("card")>=0) {document.getElementById(idName).remove();}
				else document.getElementById(idName).parentNode.remove();
			}
        } 
        function debug_element_byId(){
          var value = document.getElementById("inp_test").value;
          console.log( document.getElementById(value));
        }
        function debug_element_createAll(){
			insertAdjacentHTML_inpuText ();
			insertAdjacentHTML_inpuRange ();
			insertAdjacentHTML_sidenavItemTxt ();
			insertAdjacentHTML_button ();        	
        }
        function debug_requestSend(){
          var value = document.getElementById("inp_test").value;
          socket_send(value);
        }
        function debug_toggle(){
        	response_display = !response_display;
        	socket_display = !socket_display;

        	console.log("socket_display: " + socket_display +" response_display : " + response_display);
        }   
        function debug_show(){
       		document.getElementById("region_debug").style.display == "" ? document.getElementById("region_debug").style.display = "none" : document.getElementById("region_debug").style.display = "";
        }   



// insertAdjacentHTML_card();
// insertAdjacentHTML_card();
// insertAdjacentHTML_card();
// expected output: Array ["The quick brown fox jumps over the lazy dog."]

// var test = '{"create":{"wifi_reset":{"0":4,"1":"input-text-4","2":"button-4","3":"wifi_reset","4":"setup","5":"wifi_reset","6":""},"wifi_dnsname":{"0":4,"1":"input-text-5","2":"button-5","3":"wifi_dnsname","4":"MY_ESPWEBSERVER","5":"wifi_dnsname","6":""},"wifi_hostname":{"0":4,"1":"input-text-6","2":"button-6","3":"wifi_hostname","4":"MY_ESPWEBSERVER","5":"wifi_hostname","6":""},"wifi_ip":{"0":4,"1":"input-text-7","2":"button-7","3":"wifi_ip","4":"192.168.0.232","5":"wifi_ip","6":""},"wifi_ssid":{"0":4,"1":"input-text-8","2":"button-8","3":"wifi_ssid","4":"freebox_123_EXT","5":"wifi_ssid","6":""},"wifi_psk":{"0":4,"1":"input-text-9","2":"button-9","3":"wifi_psk","4":"phcaadax","5":"wifi_psk","6":""},"wifiAp_psk":{"0":4,"1":"input-text-10","2":"button-10","3":"wifiAp_psk","4":"mywifiappsk","5":"wifiAp_psk","6":""},"wifiAp_ssid":{"0":4,"1":"input-text-11","2":"button-11","3":"wifiAp_ssid","4":"a��?MY_ESPWEBSERVER","5":"wifiAp_ssid","6":""},"wifi_subnet":{"0":4,"1":"input-text-12","2":"button-12","3":"wifi_subnet","4":"255.255.255.0","5":"wifi_subnet","6":""},"wifi_gateway":{"0":4,"1":"input-text-13","2":"button-13","3":"wifi_gateway","4":"192.168.0.254","5":"wifi_gateway","6":""}}}';
// var response = JSON.parse(test);
// inputText_create(response, 3, "debug");
// test = '{"update":{"element":"sidebarLblValue","id_1":"sidenav-item-text-0","id_2":"sidenav-item-text-value-0","val_1":"value_1","val_2":"value_2","name_1":"onTime","name_2":""}}';
// test = '{"remove":{"card-0":"card-0", "card-1":"card-1"}}';
// response = JSON.parse(test);
// removeElements(response, "de");
// sidebarLblValue_update(response, "debug");
// 	###################################################################################### 
//	INITIIALISE JS
//	
socket_initialize();
  
  )=====";

#endif // JS_CORE_H
