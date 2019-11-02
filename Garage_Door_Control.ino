#include <ESP8266SSDP.h>
#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library
#include <PubSubClient.h>         //MQTT
 
////**********START CUSTOM PARAMS******************//
 
// Host name and router info
const char* host = "Garage1ESP";
const char* ssid = "CJG_GbE_2G4";
const char* password = "GlockHK23";
 
//Define the pins
#define RELAY_PIN     D1            // Digital pin that connect to the coil of the relay on the shield
#define PUSH_DWELL_US 900           // Dwell time, in us, for 'holding' down the button
 
//Define MQTT parameters 
#define mqtt_server "192.168.0.249"           // This is the host address of HASSIO (reserved address in router)
#define button_topic "sensor/garage/action1"  // You can change this name, but make sure you "replace all"
const char* mqtt_user = "homeassistant";      // As defined in configuration.YAML
const char* mqtt_pass = "GlockHK23";          // As defined in configuration.YAML
 
//************END CUSTOM PARAMS********************//
//This can be used to output the date the code was compiled
// const char compile_date[] = __DATE__ " " __TIME__;
 
WiFiClient espCgarage1;
 
//Initialize MQTT
PubSubClient client(espCgarage1);
 
//Setup Variables
String switch1;
String strTopic;
 
void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
 
  Serial.begin(115200);
  Serial.println("Reset");

  setup_wifi();

  client.setServer(mqtt_server, 1883);  //1883 is the port number you have forwared for mqtt messages.
  client.setCallback(callback);         //callback is the function that gets called for a topic sub
}


void loop() {
  //If MQTT client can't connect to broker, then reconnect
  if (!client.connected()) {
    Serial.println("Reconnecting...");
    setup_wifi();                         // The router could have kicked us off...
    reconnect();
  }
  client.loop(); //the mqtt function that processes MQTT messages
  
}
 
void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  strTopic = String((char*)topic);
  if (strTopic == button_topic)
  {
    switch1 = String((char*)payload);
    Serial.println(switch1);
    if (switch1 == "PUSH_BUTTON")
    {
      // 'Push' the garage door open/close button
      Serial.println("ON");
      digitalWrite(RELAY_PIN, HIGH);
      delay(PUSH_DWELL_US);
      digitalWrite(RELAY_PIN, LOW);
    }
  }
}

void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  //Reconnect to Wifi and to MQTT. If Wifi is already connected, then autoconnect doesn't do anything.
  Serial.print("Attempting MQTT connection...");
  if (client.connect(host, mqtt_user, mqtt_pass)) {
    Serial.println("connected");
    client.subscribe("sensor/garage/#");
  } 
  else {
    Serial.print("failed, rc=");
    Serial.print(client.state());
    Serial.println(" try again in 5 seconds");
    // Wait 5 seconds before retrying
    delay(5000);
  }
}
