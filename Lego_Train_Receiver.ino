//#include <ESP8266WiFi.h>
#include <WiFi.h>
#include <PubSubClient.h>


#define forward D1
#define backward D2

#define MQTT_AUTH true
#define MQTT_USERNAME "USERNAME"
#define MQTT_PASSWORD "PASSWORD"

const char* ssid = "SSID";
const char* password = "SECRET";

const String HOSTNAME  = "HOSTNAME";
const String MQTT_TRAIN_TOPIC_SPEED = "SPEED_TOPIC";
const String WILL_TOPIC = "devices/will";
const String WILL_MESSAGE = HOSTNAME + " Disconnected ";
const char* MQTT_SERVER = "SERVER";

WiFiClient wclient;
PubSubClient client(MQTT_SERVER,1883,wclient);

//-------------------------------------------
void move_forward(int speed){
  digitalWrite(backward,LOW);
  Serial.println("FORWARD");
  int value = (255/7)*speed;
  Serial.println(value);
  analogWrite(forward,value);
  client.publish(MQTT_TRAIN_STATE_TOPIC.c_str(),"FORWARD",true);
}
//-------------------------------------------
void move_backward(int speed){
  digitalWrite(forward,LOW);
  int value = (255/7)*speed;
  Serial.println(value);
  analogWrite(backward,value);  
  Serial.println("BACK");
  client.publish(MQTT_TRAIN_STATE_TOPIC.c_str(),"BACKWARD", true);
}
//-------------------------------------------
void stop(){
  digitalWrite(backward,LOW);
  digitalWrite(forward,LOW);
  Serial.println("STOP");
  client.publish(MQTT_TRAIN_STATE_TOPIC.c_str(),"STOP", true);
}
//-------------------------------------------
void callback(char* topic, byte* payload, unsigned int length) {
  String payloadStr = "";
  for (int i=0; i<length; i++) {
    payloadStr += (char)payload[i];
  }
  String topicStr = String(topic);
  String dir = payloadStr.substring(0,1);
  String speed = payloadStr.substring(2,3);
  Serial.println(dir);
  Serial.println(speed);
  if(dir.equals("F")){
    move_forward(speed.toInt());
    Serial.println("FORWARD");
  }
  if(dir.equals("B")){
    move_backward(speed.toInt());
    Serial.println("BACKWARD");        
  }
  Serial.println("end");
}
//-------------------------------------------
bool checkMqttConnection(){
  if (!client.connected()) {
    if ( client.connect( HOSTNAME.c_str(),MQTT_USERNAME, MQTT_PASSWORD ) ) {; //, WILL_TOPIC, 0, 0, WILL_MESSAGE)) {
      //SUBSCRIÇÃO DE TOPICOS
      Serial.println("CONNECTED");
      client.subscribe(MQTT_TRAIN_TOPIC_SPEED.c_str());
      //client.subscribe(MQTT_TRAIN_TOPIC_DIRECTION.c_str());
    }
  }
  return client.connected();
}


//-------------------------------------------

void connectWifi(){
  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);
  Serial.println(".");
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

void setup() {
  Serial.begin(115200);
  client.setCallback(callback);  
  pinMode(forward, OUTPUT);
  pinMode(backward, OUTPUT);  
  connectWifi();  

}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    if (checkMqttConnection()){
      client.loop();
    }
  }else{
    connectWifi();
  }

}
