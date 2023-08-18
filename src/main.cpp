/////////////////////////////////////////////////////////////////////////////////////////
// Includes 1
/////////////////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <uri/UriBraces.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Pages.h>

/////////////////////////////////////////////////////////////////////////////////////////
//Defines
/////////////////////////////////////////////////////////////////////////////////////////
//OLED
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

// Definir o login e senha do WIFI
#define WIFI_SSID "Gustavo"
#define WIFI_PASSWORD "97752117"
#define WIFI_SSID1 "SIAM_UNIFI"
#define WIFI_PASSWORD2 "siam2k19"
// Defining the WiFi channel speeds up the connection
#define WIFI_CHANNEL 6

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

/////////////////////////////////////////////////////////////////////////////////////////
// Definir Variaveis Para indicar o que cada pino vai ser
/////////////////////////////////////////////////////////////////////////////////////////
//pinos que estão no Driver do motor do carrinho
const int DirFrente = 26;
const int DirTras = 27;
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
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
    Serial.print("Connecting to WiFi ");
  display.println("Connecting to wifi:");
  Serial.print(WIFI_SSID);
  display.display(); // actual
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    display.setCursor(0,0);
    display.println("Connecting to wifi:");
    display.print(WIFI_SSID);
    display.display(); // actual
    display.setCursor(0,15);
    display.println("Status: ");
    display.print(StsMeaning);
    display.display(); // actual
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected!");
  delay(1000);
  display.clearDisplay();
  display.setCursor(0,0);  
  Serial.print("IP address: ");
  Serial.print(WiFi.localIP());
  display.println("Connected!");
  display.print("IP: ");
  display.println(WiFi.localIP());
  display.display(); // actual



  //Caso seja acessado o IP, onde o final vai ser "/" manda a página web tradicional
  server.on("/", sendHtml);

  //Caso esteja em alguma das páginas envia novamente e printa o a ação no monitor
  server.on(UriBraces("/toggle/{}"), []() {
    String led = server.pathArg(0);
    Serial.print("Ligar motor #");
    Serial.println(led);


//        1= frente
//        2= esq
//        3= Stop
//        4= Dir
//        5= Back
    switch (led.toInt()) {
      case 1:
        FrtStatus = !FrtStatus;
        BckStatus = false;
        DirStatus = false;
        EsqStatus = false;
        StpStatus = false;
        digitalWrite(DirFrente, FrtStatus);
        digitalWrite(EsqFrente, FrtStatus);
        digitalWrite(DirTras, LOW);
        digitalWrite(EsqTras, LOW);
        break;
      case 2:
        EsqStatus = !EsqStatus;
        FrtStatus = false;
        BckStatus = false;
        DirStatus = false;
        StpStatus = false;
        digitalWrite(EsqFrente, EsqStatus);
        digitalWrite(DirFrente, LOW);
        digitalWrite(DirTras, LOW);
        digitalWrite(EsqTras, LOW);
        break;
      case 3:
        StpStatus = !StpStatus;
        FrtStatus = false;
        BckStatus = false;
        DirStatus = false;
        EsqStatus = false;
        digitalWrite(EsqFrente, LOW);
        digitalWrite(DirFrente, LOW);
        digitalWrite(DirTras, LOW);
        digitalWrite(EsqTras, LOW);
        break;
      case 4:
        DirStatus = !DirStatus;
        FrtStatus = false;
        BckStatus = false;
        EsqStatus = false;
        StpStatus = false;
        digitalWrite(EsqFrente, LOW);
        digitalWrite(DirFrente, DirStatus);
        digitalWrite(DirTras, LOW);
        digitalWrite(EsqTras, LOW);
        break;
      case 5:
        BckStatus = !BckStatus;
        FrtStatus = false;
        DirStatus = false;
        EsqStatus = false;
        StpStatus = false;
        digitalWrite(EsqFrente, LOW);
        digitalWrite(DirFrente, LOW);
        digitalWrite(DirTras, BckStatus);
        digitalWrite(EsqTras, BckStatus);
        break;
    }

    sendHtml();
  });

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
  Serial.print(VinRes);
  Serial.println("V");
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
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
 
  // Prints the distance in the OLED LCD
  display.setCursor(70,15); 
  display.setTextColor(WHITE, BLACK); 
  display.println(distanceCm);
}



/////////////////////////////////////////////////////////////////////////////////////////
// Rotina principal que vai ficar loopando
/////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  
  server.handleClient();
  delay(200); 
  timr0 = (timr1 - millis());
  if (timr0 < -2000)
  {
  timr1 = millis();  
  ReadVin();
  ReadDis();

  }

  
}