#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include "SSD1306.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//Adafruit_SSD1306  display(0x3C, D2, D1);

#define MQTT_AUTH true
#define MQTT_USERNAME "USERNAME"
#define MQTT_PASSWORD "PASSWORD"

#define direction D5
#define stop D6 

const char* ssid = "SSID";
const char* password = "SECRET";

const String HOSTNAME  = "HOSTNAME";
const String MQTT_TRAIN_TOPIC_SPEED = "SPEED_TOPIC";
const String WILL_TOPIC = "devices/will";
const String WILL_MESSAGE = HOSTNAME + " Disconnected ";
const char* MQTT_SERVER = "SERVER";

String speed = "0";
String dir = "0";
String value = "0";
int stop_status = 0;

WiFiClient wclient;
PubSubClient client(MQTT_SERVER,1883,wclient);


void showValue(){
  display.clearDisplay();
  display.setTextSize(4); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(45, 25);
  display.println(speed);
  display.display();      // Show initial text
  delay(100);
}

void checkValue(){
    int potencia = analogRead(A0);
    if( (potencia>=0) and (potencia<=146) ){
      speed = "1";  
    }
    if( (potencia>=147) and (potencia<=292) ){
      speed = "2";  
    }
    if( (potencia>=293) and (potencia<=438) ){
      speed = "3";  
    }
    if( (potencia>=439) and (potencia<=584) ){
      speed = "4";  
    }
    if( (potencia>=585) and (potencia<=730) ){
      speed = "5";  
    }
    if( (potencia>=731) and (potencia<=876) ){
      speed = "6";  
    }
    if( (potencia>=877) and (potencia<=1024) ){
      speed = "7"; 
    }
    
    if(digitalRead(direction) == HIGH){  
      Serial.println("Forward");
      dir = "F";
    }
    if(digitalRead(direction)==LOW){
      Serial.println("Backward");
      dir = "B";
    }
  value = dir+"_"+speed;
  Serial.println(value);
  client.publish(MQTT_TRAIN_TOPIC_SPEED.c_str(),value.c_str(),true);
  showValue();
  delay(500);
}

//-------------------------------------------
bool checkMqttConnection(){
  if (!client.connected()) {
    if ( client.connect( HOSTNAME.c_str(),MQTT_USERNAME, MQTT_PASSWORD ) ) {; //, WILL_TOPIC, 0, 0, WILL_MESSAGE)) {
      //SUBSCRIÇÃO DE TOPICOS
      Serial.println("CONNECTED");
      //client.subscribe(MQTT_TRAIN_TOPIC.c_str());
    }
  }
  return client.connected();
}
//-------------------------------------------
void connectWifi(){
  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);
  Serial.println(".");
  // Aguarda até estar ligado ao Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Ligado a ");
  Serial.println(ssid);
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

//-------------------------------------------
void setup()
{
  Serial.begin(115200);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();
  connectWifi();
  pinMode(A0, INPUT); 
  pinMode(direction, INPUT_PULLUP);
  pinMode(stop, INPUT_PULLUP);
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED) {
    if (checkMqttConnection()){
      checkValue();
    }
  }else{
    connectWifi();
  }  
 
}
