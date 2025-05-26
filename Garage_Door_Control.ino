#include <ESP8266SSDP.h>
#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library
#include <PubSubClient.h>         //MQTT
 
/**************************************************/
////**********START CUSTOM PARAMS******************//
/**************************************************/
//TODO develop a state message for the purpose of sending out a beacon 
#define SW_VERSION    "3.0.0"

// Host name and router info

String host_string;      
char host[16];    

/**
 * Based on the config ID (jumper settings), assign
 * the button topic this module should respond to
 */
char assigned_button_topic[64];  // Based on the config ID, define the   

/**
 * WiFi parameters
 */

const char* ssid = "CJG_GbE_2G4";
const char* password = "GlockHK23";
 
/** 
 * Define pins for driving the 
 * LED and activating the relay
*/
#define RELAY_PIN       D0            // Digital pin that connects to the coil of the relay on the shield
#define LED_PIN         D1            // Drive panel LED for health indication


/*
 * ID bits D6 and D5 are used to
 * identify which garage door 
 * the controller is associated with.
 * 
 *   Bits  +
 *   D6 D5 | Garage Door
 * +-------------------------------+
 *    0  0 | House Single Door 
 *    0  1 | House Double Door
 *    1  0 | Shop Door West
 *    1  1 | Shop Door East
 *         +
 *
 */

#define SGL_SIDE        0             // Bits D6 and D5 = 0b00
#define DBL_SIDE        1             // Bits D6 and D5 = 0b01
#define SHOP_DOOR_WEST  2             // Bits D6 and D5 = 0b10
#define SHOP_DOOR_EAST  3             // Bits D6 and D5 = 0b11

#define PUSH_DWELL_US   1200          // Dwell time, in us, for 'holding' down the button
 
//Define MQTT parameters 
#define mqtt_server "192.168.0.249"   // This is the host address of HASSIO (reserved address in router)

const char* BUTTON_TOPIC_SGL        = "sensor/garage/sgl_action";   
const char* BUTTON_TOPIC_DBL        = "sensor/garage/dbl_action";           
const char* BUTTON_TOPIC_SHOP_WEST  = "sensor/garage/shop_west_action";
const char* BUTTON_TOPIC_SHOP_EAST  = "sensor/garage/shop_east_action";           

const char* mqtt_user = "cjg_mqtt";      // As defined in configuration.YAML
const char* mqtt_pass = "MQTTRockRiverHK23";          // As defined in HASSIO integration setup

/**************************************************/
//************END CUSTOM PARAMS********************//
/**************************************************/

WiFiClient espCgarage1;
 
//Initialize MQTT
PubSubClient client(espCgarage1);
 
//Setup Variables
String          switch1;
String          strTopic;
int             configured_side_int = 99;
String          configured_side_str;
 
void setup() {
  
  /**
   * Relay PWM shall be off
   */
  analogWrite(RELAY_PIN, 0);
  
  /**
   * Initialize LED pin
   * LED is ON when connected
   * to MQTT broker
   */
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  /* Define ID pins as inputs */
  pinMode(D5, INPUT);
  pinMode(D6, INPUT);
  
  Serial.begin(115200);
  delay(5000);    // Give the user some time to open the serial monitor
  Serial.println("Reset");

  configured_side_int = (int)(digitalRead(D6) << 1 | digitalRead(D5));   
  
  /**
   * Determine the host name
   * based on the "configured side" 
   * configuration bits.  The String()
   * function doesn't produce a Std C-style
   * string, thus the reason for calling 
   * method .c_str(), which returns a const char 
   * pointer that the strcpy() function gladly accepts
   * If simply passing in the host_string, without 
   * calling the method, the compiler will produce
   * an error that reads something like "cannot convert
   * String to Const Char *" 
   * 
   */
  host_string = (String("GarageCtrl" + String(configured_side_int)));
  Serial.print("Host name: "); Serial.println(host_string);
  strcpy(host, host_string.c_str());
  
  switch (configured_side_int) {
    case SGL_SIDE:
      strcpy(assigned_button_topic, BUTTON_TOPIC_SGL);
      Serial.println("ID configired as Clinton's side.");
      Serial.print("Assigned button topic: ");  Serial.println(assigned_button_topic);
    break;
    
    case DBL_SIDE:
      strcpy(assigned_button_topic, BUTTON_TOPIC_DBL);
      Serial.println("ID configired as Meghan's side.");
      Serial.print("Assigned button topic: ");  Serial.println(assigned_button_topic);
    break;
    
    case SHOP_DOOR_WEST:
      strcpy(assigned_button_topic,BUTTON_TOPIC_SHOP_WEST);
      Serial.println("ID configired as shop door West.");
      Serial.print("Assigned button topic: ");  Serial.println(assigned_button_topic);
    break;
    
    case SHOP_DOOR_EAST:
      strcpy(assigned_button_topic,BUTTON_TOPIC_SHOP_EAST);
      Serial.println("ID configired as shop door East.");
      Serial.print("Assigned button topic: ");  Serial.println(assigned_button_topic);
    break;
    
    default:
      Serial.println("ID improperly configured.");
    break;
  }
  
  Serial.print("The SW version: ");
  Serial.print(SW_VERSION);
  Serial.println();

  setup_wifi();

  client.setServer(mqtt_server, 1883);  //1883 is the port number you have forwared for mqtt messages.
  client.setCallback(callback);         //callback is the function that gets called for a topic sub
  
}


void loop() {
  //If MQTT client can't connect to broker, then reconnect
  if (!client.connected()) {
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
  
  if (strTopic == assigned_button_topic)
  {
    switch1 = String((char*)payload);
    Serial.print("Topic: "); Serial.println(strTopic);
    Serial.print("Payload: ");Serial.println(switch1);
    if (switch1 == "PUSH_BUTTON")
    {
      // 'Push' the garage door open/close button
      Serial.println("RELAY ACTIVE");
      analogWrite(RELAY_PIN,512);       // Duty cycle val 0-1023 (512~50%)  
      delay(PUSH_DWELL_US);
      analogWrite(RELAY_PIN, 0);
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
