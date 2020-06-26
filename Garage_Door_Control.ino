#include <ESP8266SSDP.h>
#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library
#include <PubSubClient.h>         //MQTT
 
////**********START CUSTOM PARAMS******************//
 
// Host name and router info
const char* host = "Garage1ESP";
const char* ssid = "CJG_GbE_2G4";
const char* password = "GlockHK23";
 
//Define the pins
#define RELAY_PIN       D0            // Digital pin that connects to the coil of the relay on the shield
#define LED_PIN         D1            // Drive panel LED for health indication


/*
 * ID bits D4 and D5 are used to
 * identify which garage door 
 * the controller is associated with.
 * 
 *   Bits  +
 *   D6 D5 | Garage Door
 * +-------------------------------+
 *    0  0 | Clinton's Side
 *    0  1 | Meghan's Side
 *    1  0 | Shop Door West
 *    1  1 | Shop Door East
 *         +
 *
 */

#define CJG_SIDE        0             // Bits D3 and D2 = 0b00
#define MEG_SIDE        1             // Bits D3 and D2 = 0b01
#define SHOP_DOOR_WEST  2             // Bits D3 and D2 = 0b10
#define SHOP_DOOR_EAST  3             // Bits D3 and D2 = 0b11

#define PUSH_DWELL_US 900           // Dwell time, in us, for 'holding' down the button
 
//Define MQTT parameters 
#define mqtt_server "192.168.0.249"                   // This is the host address of HASSIO (reserved address in router)

#define BUTTON_TOPIC_CJG        "sensor/garage/cjg_action"   
#define BUTTON_TOPIC_MEG        "sensor/garage/meg_action"           
#define BUTTON_TOPIC_SHOP_WEST  "sensor/garage/shop_west_action"           
#define BUTTON_TOPIC_SHOP_EAST  "sensor/garage/shop_east_action"           


const char* mqtt_user = "homeassistant";      // As defined in configuration.YAML
const char* mqtt_pass = "GlockHK23";          // As defined in configuration.YAML
 
//************END CUSTOM PARAMS********************//
//This can be used to output the date the code was compiled
// const char compile_date[] = __DATE__ " " __TIME__;
 
WiFiClient espCgarage1;
 
//Initialize MQTT
PubSubClient client(espCgarage1);
 
//Setup Variables
String          switch1;
String          strTopic;
int             configured_side = 99;
 
void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  /* Define ID pins as inputs */
  pinMode(D5, INPUT);
  pinMode(D6, INPUT);
 
  Serial.begin(115200);
  Serial.println("Reset");

  configured_side = (int)(digitalRead(D6) << 1 | digitalRead(D5));
  
  switch (configured_side) {
    case CJG_SIDE:
      Serial.println("ID configired as Clinton's side.");
    break;
    case MEG_SIDE:
      Serial.println("ID configired as Meghan's side.");
    break;
    case SHOP_DOOR_WEST:
      Serial.println("ID configired as shop door West.");
    break;
    case SHOP_DOOR_EAST:
      Serial.println("ID configired as shop door East.");
    break;
    default:
      Serial.println("ID improperly configured.");
    break;
  }
  
  setup_wifi();

  client.setServer(mqtt_server, 1883);  //1883 is the port number you have forwared for mqtt messages.
  client.setCallback(callback);         //callback is the function that gets called for a topic sub
}


void loop() {
  //If MQTT client can't connect to broker, then reconnect
  while (!client.connected()) {
    digitalWrite(LED_PIN, LOW);            
    Serial.println("Reconnecting...");
    setup_wifi();                         // The router could have kicked us off...
    reconnect();
    delay(500);
  }
  
  digitalWrite(LED_PIN, HIGH);    
  client.loop();                  // the mqtt function that processes MQTT messages
  
}
 
void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  strTopic = String((char*)topic);
  if (strTopic == BUTTON_TOPIC_CJG && configured_side == CJG_SIDE)
  {
    switch1 = String((char*)payload);
    Serial.print("Topic: "); Serial.println(strTopic);
    Serial.print("Payload: ");Serial.println(switch1);
    if (switch1 == "PUSH_BUTTON")
    {
      // 'Push' the garage door open/close button
      Serial.println("RELAY ACTIVE");
      digitalWrite(RELAY_PIN, HIGH);
      delay(PUSH_DWELL_US);
      digitalWrite(RELAY_PIN, LOW);
      Serial.println("RELAY INACTIVE");
    }
  }

  else if (strTopic == BUTTON_TOPIC_MEG && configured_side == MEG_SIDE)
  {
    switch1 = String((char*)payload);
    Serial.print("Topic: "); Serial.println(strTopic);
    Serial.print("Payload: ");Serial.println(switch1);
    if (switch1 == "PUSH_BUTTON")
    {
      // 'Push' the garage door open/close button
      Serial.println("RELAY ACTIVE");
      digitalWrite(RELAY_PIN, HIGH);
      delay(PUSH_DWELL_US);
      digitalWrite(RELAY_PIN, LOW);
      Serial.println("RELAY INACTIVE");
    }
  }
  
  else if (strTopic == BUTTON_TOPIC_SHOP_WEST && configured_side == SHOP_DOOR_WEST)
  {
    switch1 = String((char*)payload);
    Serial.print("Topic: ");    Serial.println(strTopic);
    Serial.print("Payload: ");  Serial.println(switch1);
    if (switch1 == "PUSH_BUTTON") {
      // 'Push' the garage door open/close button
      Serial.println("RELAY ACTIVE");
      digitalWrite(RELAY_PIN, HIGH);
      delay(PUSH_DWELL_US);
      digitalWrite(RELAY_PIN, LOW);
      Serial.println("RELAY INACTIVE");
    }
  }
  
  else if (strTopic == BUTTON_TOPIC_SHOP_EAST && configured_side == SHOP_DOOR_EAST)
  {
    switch1 = String((char*)payload);
    Serial.print("Topic: ");    Serial.println(strTopic);
    Serial.print("Payload: ");  Serial.println(switch1);
    if (switch1 == "PUSH_BUTTON") {
      // 'Push' the garage door open/close button
      Serial.println("RELAY ACTIVE");
      digitalWrite(RELAY_PIN, HIGH);
      delay(PUSH_DWELL_US);
      digitalWrite(RELAY_PIN, LOW);
      Serial.println("RELAY INACTIVE");
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
