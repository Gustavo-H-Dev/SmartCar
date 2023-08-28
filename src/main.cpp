/////////////////////////////////////////////////////////////////////////////////////////
// Includes 1
/////////////////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiMulti.h>
#include <WebServer.h>
#include <uri/UriBraces.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Pages.h>

/////////////////////////////////////////////////////////////////////////////////////////
//Defines
/////////////////////////////////////////////////////////////////////////////////////////
//Define se o ambiente é de teste virtual
//#define VIRTUAL

//OLED
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16



// Defining the WiFi channel speeds up the connection
#define WIFI_CHANNEL 6

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701


/////////////////////////////////////////////////////////////////////////////////////////
// Objetos para uso das bibliotecas
/////////////////////////////////////////////////////////////////////////////////////////
// para ser usado conexão de varios Wifis
WiFiMulti wifiMulti;

/////////////////////////////////////////////////////////////////////////////////////////
// Definir Variaveis Para indicar o que cada pino vai ser
/////////////////////////////////////////////////////////////////////////////////////////
//pinos que estão no Driver do motor do carrinho
const int DirFrente = 26;
const int DirTras = 27;

//const int LED1 = 26;
//const int LED2 = 27;

//bool led1State = false;
//bool led2State = false;

const int EsqFrente = 13;
const int EsqTras = 12;
//Sensor de distancia
const int trigPin = 18;
const int echoPin = 19;
//ADC
const int Vin = 35;
int VinVal = 0;

/////////////////////////////////////////////////////////////////////////////////////////
// Definir Variaveis para para registar dados 
/////////////////////////////////////////////////////////////////////////////////////////
//ADC
double VinRes = 0;

//Sensor de distância
long duration;
float distanceCm;
float distanceInch;

//Timers
int timr0 = 0;
int timr1 = 0;

//Wifi
int StsWifi = 0;
String StsMeaning = "Hello";

//http request
String Joy1Dir = "Note";
String Joy2Dir = "Note";


String outputState(int output){
  if(digitalRead(output)){
    return "checked";
  }
  else {
    return "";
  }
}

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if(var == "BUTTONPLACEHOLDER"){
    String buttons = "<p></p>";
    //buttons += "<h4>Output - GPIO 2</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"2\" " + outputState(2) + "><span class=\"slider\"></span></label>";
    //buttons += "<h4>Output - GPIO 4</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"4\" " + outputState(4) + "><span class=\"slider\"></span></label>";
    //buttons += "<h4>Output - GPIO 33</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"33\" " + outputState(33) + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  return String();
}



/////////////////////////////////////////////////////////////////////////////////////////
//Rotina de configuração inicial 
/////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
//Monitor Serial Config
// Iniciando A comunicação serial, definindo o baud rate igual ao da ESP32
   Serial.begin(9600);


 //Definindo os pinos como saída
  pinMode(DirFrente, OUTPUT);// Definindo os pinos dos motores 
  pinMode(DirTras, OUTPUT);// Definindo os pinos dos motores
  pinMode(EsqFrente, OUTPUT);// Definindo os pinos dos motores
  pinMode(EsqTras, OUTPUT);// Definindo os pinos dos motores
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  //Definindo as entradas
  pinMode(Vin,INPUT_PULLUP);//Pino que está ligado no Vin com resistores para saber a tensão
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

// PWM COnfig
  ledcSetup(0, 1000, 10);//Atribuimos ao canal 0 a frequencia de 1000Hz com resolucao de 10bits. 

//OLED CONFI
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  delay(2000); // Pause for 2 seconds
   display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);

//Iniciando o WIFI
// Definir o login e senha do WIFI
#ifndef VIRTUAL
    wifiMulti.addAP("Gustavo", "97752117");
    wifiMulti.addAP("SIAM_UNIFI", "siam2k19");
    wifiMulti.addAP("Machado", "99053909");
    wifiMulti.addAP("abc", "12345678");
    wifiMulti.addAP("Casa1", "180934eb");
    wifiMulti.addAP("Wokwi-GUEST", "");
    Serial.println("Connecting Wifi...");
    display.println("Connecting to wifi:");
    display.display();
    while(wifiMulti.run() != WL_CONNECTED) {
       delay(5000);        
       Serial.print(".");
    }
    Serial.print("Não virtual");
#else
  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.print("Virtual");
#endif
  Serial.println(" Conectado!");    
    Serial.print("\n");
    Serial.print("WiFi connected : ");
    Serial.print(WiFi.SSID());
    Serial.print("IP address : ");
    Serial.println(WiFi.localIP());

    display.clearDisplay();
    display.display();
    display.setCursor(0,0);
    display.print("Rede:");
    display.println(WiFi.SSID());
    display.print("IP: ");
    display.println(WiFi.localIP());
    display.display(); // actual

/////////////////////////////////////////////////////////////////////////////////////////
//Gerencia da página Web
/////////////////////////////////////////////////////////////////////////////////////////
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // disponibiliza o url "/" por acesso somente via POST
  server.on("/", HTTP_POST, [](AsyncWebServerRequest * request) {
    // Verifique se o parâmetro POST existe ou não
    if (request->hasParam("Joy1", true)) {
      AsyncWebParameter* p = request->getParam("Joy1", true);
      Joy1Dir = ("%s\n", p->name().c_str(), p->value().c_str());
      //Serial.print("O Valor de Joy1= ");
      //Serial.println(Joy1Dir);
      
      AsyncWebParameter* q = request->getParam("Joy2", true);
      Joy2Dir =  ("%s\n", q->name().c_str(), q->value().c_str());
      //Serial.print("O Valor de Joy2= ");
      //Serial.println(Joy2Dir);
      //Joy2Dir = Joy2Dir.toInt();
    }
    
    else {
      Serial.print("O parâmetro POST %s nome-qualquer não existe nesta requisição %s\n \n");
    }
    request->send(200);
  });
    // Start server
  server.begin();
}

/////////////////////////////////////////////////////////////////////////////////////////
//Traduz status do WiFi
/////////////////////////////////////////////////////////////////////////////////////////
void ReadWifiStatus()
{ 
  StsWifi = (WiFi.status());
  switch(StsWifi) {
    case 255:
    StsMeaning = "Sem Shield";
    break;
    
    case 0:
    StsMeaning = "Aguardando Conexão";
    break;
    
    case 1:
    StsMeaning = "Rede não encontrada";
    break;
    
    case 2:
    StsMeaning = "Scan Completo";
    break;
    
    case 3:
    StsMeaning = "Conectado";
    break;
    
    case 4:
    StsMeaning = "Conexão Falhou";
    break;

    case 5:
    StsMeaning = "Conexão perdida";
    break;

    case 6:
    StsMeaning = "Desconectado";
    break;

    default:
    StsMeaning = "Desconhecido";
    break;
}
}



/////////////////////////////////////////////////////////////////////////////////////////
// Rotina para Ler a distância
/////////////////////////////////////////////////////////////////////////////////////////
void ReadVin ()
{
  VinVal = analogRead(Vin);
  VinRes=map(VinVal,0,4095,0,50);
  VinRes= (VinRes/10);
  //Mostrar no serial
  //Serial.print(VinRes);
  //Serial.println("V");
  display.setTextColor(WHITE, BLACK);
  display.setCursor(0,15);  
  display.print(VinRes);
  display.print("V");
  display.display(); 
}


/////////////////////////////////////////////////////////////////////////////////////////
// Rotina para Ler a distância
/////////////////////////////////////////////////////////////////////////////////////////
void ReadDis()
{
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  

  // Prints the distance in the Serial Monitor
  //Serial.print("Distance (cm): ");
  //Serial.println(distanceCm);
 
  // Prints the distance in the OLED LCD
  display.setCursor(70,15); 
  display.setTextColor(WHITE, BLACK); 
  display.println(distanceCm);
}


/////////////////////////////////////////////////////////////////////////////////////////
// Rotina que move os motores
/////////////////////////////////////////////////////////////////////////////////////////
void MoveCar()
{
  Serial.println("Valor de Joy1DIR= ");
  Serial.println(Joy1Dir);
 if (Joy1Dir.equals("C")) {
        digitalWrite(DirFrente, LOW);
        digitalWrite(EsqFrente, LOW);
        digitalWrite(DirTras, LOW);
        digitalWrite(EsqTras, LOW);
        Serial.println("Parado");
}
else if (Joy1Dir == "N") {
        digitalWrite(DirFrente, HIGH);
        digitalWrite(EsqFrente, HIGH);
        digitalWrite(DirTras, LOW);
        digitalWrite(EsqTras, LOW);
        Serial.println("Frente Total");        
}
else if (Joy1Dir == "NE") {
        digitalWrite(EsqFrente, HIGH);
        digitalWrite(DirFrente, LOW);
        digitalWrite(DirTras, LOW);
        digitalWrite(EsqTras, LOW);
        Serial.println("Direita frente");   
}
else if (Joy1Dir == "E") {
        digitalWrite(DirFrente, LOW);
        digitalWrite(EsqFrente, HIGH);
        digitalWrite(DirTras, HIGH);
        digitalWrite(EsqTras, LOW);
        Serial.println("Giro Á direita");  
}
else if (Joy1Dir == "SE") {
        digitalWrite(DirFrente, LOW);
        digitalWrite(EsqFrente, LOW);
        digitalWrite(DirTras, LOW);
        digitalWrite(EsqTras, HIGH);
        Serial.println("TRAS Direita");    
}
else if (Joy1Dir == "S") {
        digitalWrite(DirFrente, LOW);
        digitalWrite(EsqFrente, LOW);
        digitalWrite(DirTras, HIGH);
        digitalWrite(EsqTras, HIGH);
        Serial.println("TRAS");
}
else if (Joy1Dir == "SW") {
        digitalWrite(DirFrente, LOW);
        digitalWrite(EsqFrente, LOW);
        digitalWrite(DirTras, HIGH);
        digitalWrite(EsqTras, LOW);
        Serial.println("Tras esquerda");    
}
else if (Joy1Dir == "W") {
        digitalWrite(EsqFrente, LOW);
        digitalWrite(DirFrente, HIGH);
        digitalWrite(DirTras, LOW);
        digitalWrite(EsqTras, HIGH);
        Serial.println("Giro a esquerda");  
}
else if (Joy1Dir == "NW") {
        digitalWrite(DirFrente, HIGH);
        digitalWrite(EsqFrente, LOW);
        digitalWrite(DirTras, LOW);
        digitalWrite(EsqTras, LOW);
        Serial.println("Frente Esquerda");     
}
else {
        Serial.println("Deu ruim");      
}

  Serial.println("Fim");
  Serial.println(" ");
  Serial.println(" ");
}

////////////////////////////////////////////////////////////////////////////////////////
// Rotina principal que vai ficar loopando
/////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  
  //server.handleClient();
  delay(100); 
  timr0 = (timr1 - millis());
  if (timr0 < -2000)
  {
  timr1 = millis();  
  ReadVin();
  ReadDis();
  }
  MoveCar();
}