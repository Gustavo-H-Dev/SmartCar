#include <Arduino.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <uri/UriBraces.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


//Criando registradores
bool FrtStatus = false;
bool BckStatus = false;
bool DirStatus = false;
bool EsqStatus = false;
bool StpStatus = false;

//Dados passados para o html
const char* PARAM_INPUT_1 = "output";
const char* PARAM_INPUT_2 = "state";

// Definindo a porta
//WebServer server(80);
AsyncWebServer server(80);

//caso acesse uma página incorreta
void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>

  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">

  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 6px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 3px}
    input:checked+.slider {background-color: #b30000}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>

<body>
  <h2>ESP Web Server</h2>
  %BUTTONPLACEHOLDER%

<script src="joy.js"></script>  
<script>function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?output="+element.id+"&state=1", true); }
  else { xhr.open("GET", "/update?output="+element.id+"&state=0", true); }
  xhr.send();
}

// Create JoyStick object into the DIV 'joy1Div'
var Joy1 = new JoyStick('joy1Div');

var joy1IinputPosX = document.getElementById("joy1PosizioneX");
var joy1InputPosY = document.getElementById("joy1PosizioneY");
var joy1Direzione = document.getElementById("joy1Direzione");
var joy1X = document.getElementById("joy1X");
var joy1Y = document.getElementById("joy1Y");

setInterval(function(){ joy1IinputPosX.value=Joy1.GetPosX(); }, 50);
setInterval(function(){ joy1InputPosY.value=Joy1.GetPosY(); }, 50);
setInterval(function(){ joy1Direzione.value=Joy1.GetDir(); }, 50);
setInterval(function(){ joy1X.value=Joy1.GetX(); }, 50);
setInterval(function(){ joy1Y.value=Joy1.GetY(); }, 50);

// Create JoyStick object into the DIV 'joy2Div'
var joy2Param = { "title": "joystick2", "autoReturnToCenter": false };
var Joy2 = new JoyStick('joy2Div', joy2Param);

var joy2IinputPosX = document.getElementById("joy2PosizioneX");
var joy2InputPosY = document.getElementById("joy2PosizioneY");
var joy2Direzione = document.getElementById("joy2Direzione");
var joy2X = document.getElementById("joy2X");
var joy2Y = document.getElementById("joy2Y");

setInterval(function(){ joy2IinputPosX.value=Joy2.GetPosX(); }, 50);
setInterval(function(){ joy2InputPosY.value=Joy2.GetPosY(); }, 50);
setInterval(function(){ joy2Direzione.value=Joy2.GetDir(); }, 50);
setInterval(function(){ joy2X.value=Joy2.GetX(); }, 50);
setInterval(function(){ joy2Y.value=Joy2.GetY(); }, 50);

var joy3Param = { "title": "joystick3" };
var Joy3 = new JoyStick('joy3Div', joy3Param);

var joy3IinputPosX = document.getElementById("joy3PosizioneX");
var joy3InputPosY = document.getElementById("joy3PosizioneY");
var joy3Direzione = document.getElementById("joy3Direzione");
var joy3X = document.getElementById("joy3X");
var joy3Y = document.getElementById("joy3Y");

setInterval(function(){ joy3IinputPosX.value=Joy3.GetPosX(); }, 50);
setInterval(function(){ joy3InputPosY.value=Joy3.GetPosY(); }, 50);
setInterval(function(){ joy3Direzione.value=Joy3.GetDir(); }, 50);
setInterval(function(){ joy3X.value=Joy3.GetX(); }, 50);
setInterval(function(){ joy3Y.value=Joy3.GetY(); }, 50);
</script>
</body>
</html>
)rawliteral";



