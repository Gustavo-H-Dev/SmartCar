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



// Definindo a porta
WebServer server(80);


//Rotina que contém a página Web
void sendHtml() {
  String response = R"(
    <!DOCTYPE html><html>
      <head>
        <title>Controle De carrinho</title>
        <meta name="viewport" content="width=device-width, initial-scale=1" >
        <style>
          html { font-family: sans-serif; text-align: center; justify-content: center;}
          body { display: inline-flex; flex-direction: column;  align-content: center;}
          h1 { margin-bottom: 1.2em;  align-content: center;} 
          h2 { margin: 0; justify-content: center;  align-content: center;}
          div { display: grid; grid-template-columns: 1fr 1fr; grid-template-rows: auto auto; grid-auto-flow: column; grid-gap: 1em;  justify-content: center;}
          .btn { background-color: #5B5; border: none; color: #fff; padding: 0.5em 1em; 
                 font-size: 2em; text-decoration: none; justify-content: center;   align-content: center;}
          .btn.OFF { background-color: #333; justify-content: center; align-content: center; }
        </style>
      </head>
            
      <body>
        <h1>Controle De carrinho</h1>

        <div>
          <h2>Frente</h2>
          <a  href="/toggle/1" class="btn frente_TEXT" justify-content: center; >frente_TEXT</a>
        </div>
        <div>
          <h2>Esquerda</h2>
          <a href="/toggle/2" class="btn esq_TEXT">esq_TEXT</a>
          <h2>Parar</h2>
          <a href="/toggle/3" class="btn stp_TEXT">stp_TEXT</a>
          <h2>Direita</h2>
          <a href="/toggle/4" class="btn dir_TEXT">dir_TEXT</a>
        </div>
        <div>
          <h2>Re</h2>
          <a href="/toggle/5" class="btn re_TEXT"  >re_TEXT</a>
        </div>
      </body>
    </html>
  )";
  response.replace("esq_TEXT", EsqStatus ? "ON" : "OFF");
  response.replace("stp_TEXT", StpStatus ? "ON" : "OFF");
  response.replace("dir_TEXT", DirStatus ? "ON" : "OFF");
  response.replace("re_TEXT", BckStatus ? "ON" : "OFF");
  response.replace("frente_TEXT", FrtStatus ? "ON" : "OFF");
  server.send(200, "text/html", response);
}