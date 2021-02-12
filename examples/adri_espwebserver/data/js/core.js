
/*
*  ##########################################################  SOCKETS  ##########################################################
*/

var socket_connect_display= true;
var socket_display        = true;
var web_socket            = null;
var socket_connected      = false;
var socket_keep_alive_period = 500;
var keep_alive_enabled = false;
var json_data;
						
function socket_init(){
	console.log("socket_init");
	// web_socket = new WebSocket('ws://'+document.location.host+'/ws',['arduino']);
	web_socket = new WebSocket('ws://' + window.location.hostname + ':81/');
	web_socket.binaryType = "arraybuffer";
	web_socket.debug = true;
	console.log('ws://'+document.location.host+'/ws\n');

	web_socket.onopen = function(e) { 
		if (socket_connect_display) console.log("ws open",    e); 
		socket_connected=true;
		socket_status_display();


		if (keep_alive_enabled) keep_alive_time_out=setTimeout(socket_keep_alive, socket_keep_alive_period);
	};
	 
	web_socket.onclose = function(e){
		if (socket_connect_display) console.log("ws close", e); 
		socket_connected=false;
		socket_status_display();
		// setTimeout(function() {  socket_init()  }, 1000);
	};
	 
	web_socket.onerror = function(e){
		if (socket_connect_display) console.log("ws error",      e); 
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
			// if (socket_display) console.log("socket msg received: "+msg);
			server_response(msg);
}

function socket_send(msg) {
	if (! socket_connected ) socket_init();
		web_socket.send(msg); 
		if (socket_display) console.log("socket msg sent: "+msg);
}



function socket_status_display() {
	// if (socket_connected) document.getElementById('lbl_upd').value = "connected";
	// else document.getElementById('lbl_upd').value = "disconnected";
}


// VAR
var response_display = true;



// SOCKET REQUEST RECIEVE
function server_response(json) {
	if (json == "{\"}") return;
	if (response_display) { console.log('json:'); console.log(json); console.log('');}
	document.getElementById("p1").innerHTML = json;

	var response = JSON.parse(json);
	if (response_display) { console.log('response:'); console.log(response);}

}

// REQUEST UTILE
function literal_item(name, value) {
   var x="<"+name+">"+value+"</"+name+">";
   return x;
}


function request_http(){
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
	    if (this.readyState == 4 && this.status == 200) {
	       // Typical action to be performed when the document is ready:
	       document.getElementById("p1").innerHTML = xhttp.responseText;
	    }
	};
	xhttp.open("GET", "/hello", true);
	xhttp.send();	
}


// INITIIALISE

socket_init();


$(document).ready(function() {

})





