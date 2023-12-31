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

<!DOCTYPE HTML>
<html>
	<head>
		<title>Controle do Carrinho</title>
		<meta charset="utf-8">
		<meta name="description" content="SmartCar">
		<meta name="author" content="Gustavo Souza">
		<link rel="shortcut icon" type="image/png" href="https://bobboteck.github.io/img/roberto-damico-bobboteck.png">
		<style>
*
{
	box-sizing: border-box;
}
body
{
	margin: 0px;
	padding: 0px;
	font-family: monospace;
}
.row
{
	display: inline-flex;
	clear: both;
}
.columnLateral
{
  float: left;
  width: 15%;
  min-width: 300px;
}
.columnCetral
{
  float: left;
  width: 70%;
  min-width: 300px;
}
#joy2Div
{
	width:200px;
	height:200px;
	margin:50px
}
#joystick
{
	border: 1px solid #FF0000;
}
#joystick2
{
	border: 1px solid #0000FF;
}
		</style>
		<script src="http://bobboteck.github.io/joy/joy.js"></script>
	</head>
	<body>
	    <div>
        <img id="stream" src="http://192.168.1.8:81/stream" crossorigin="">
        </div>  
		<div> 
		<div class="row">
			<div class="columnLateral">
				<div id="joy1Div" style="width:200px;height:200px;margin:50px"></div>
				Posizione X:<input id="joy1PosizioneX" type="text" /><br />
				Posizione Y:<input id="joy1PosizioneY" type="text" /><br />
				Direzione:<input id="joy1Direzione" type="text" /><br />
				X :<input id="joy1X" type="text" /></br>
				Y :<input id="joy1Y" type="text" />
			</div>

			</div>
			<div class="columnLateral">
				<div id="joy2Div"></div>
				Posizione X:<input id="joy2PosizioneX" type="text" /></br>
				Posizione Y:<input id="joy2PosizioneY" type="text" /></br>
				Direzione:<input id="joy2Direzione" type="text" /></br>
				X :<input id="joy2X" type="text" /></br>
				Y :<input id="joy2Y" type="text" />
			</div>			
		</div>
		<script type="text/javascript">
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
var joy2Param = { "title": "joystick2", "autoReturnToCenter": true };
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



setInterval(function(){
	    var res = "Joy1="
		res += Joy1.GetDir()
		res += "&"
		res += "Joy2="
		res += Joy2.GetDir()


		var xhr = new XMLHttpRequest();
        xhr.open("POST", '/', true); // inicializa uma nova requisição, ou reinicializa uma requisição já existente.
        xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded"); // define o valor do cabeçalho de uma requisição HTTP
		xhr.send(res); } , 100);
       


	   
		</script>



	</body>
</html>

)rawliteral";



