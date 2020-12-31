# WEMOS Garage Door Assistant #

This repository contains the source file for the WEMOS D1 Mini (lite) whose job is to 'push' the garage door open/close button upon receiving the proper payload associated with the appropriate garage door topic.  This device allows for automations such that publishing the correct topic/payload will cause the garage door to open/close.  Within homeassistant, there will exist a cover for the garage door.  Total functionality requires the state of the door -- i.e. either open or close -- be known.  Knowing the state of the door is accomplished via a garage door tilt sensor, in which an automation within homeassistant publishes the current state to the appropriate garage door topic. 

It is the goal of this project to keep the following information updated.  For troubleshooting purposes, the MAC address of the WEMOS D1 Mini used within the garage door opener mechanism is DC-4F-22-CF-B5-40.  To make identifying the apparatus in the WiFi Client table effortless, its host name was renamed to *WEMOS_D1_Mini_Garage_Door*

Starting with version __2.0.0__, the unit can be configured to control one of four different garage door types.  The following table describes how configuration is accomplished by properly strapping ID pins D5 and D6.    

Bits (D6:D5)    | Garage Door
:-------------: | :--------------: 
00              | Clinton's Side
01              | Meghan's Side 
10              | Shop Door West
11              | Shop Door East


### TOOL SUITE ###

The source file(s) are to be compiled using Arduino IDE v1.8.8, or newer.    

It is necessary to include esp8266 libraries by including the URL to the repo.  Navigate to File > Preferences, and add the following URL: http://arduino.esp8266.com/stable/package_esp8266com_index.json

After pointing to esp8266 libraries, it is necessary to go to tools > board > boards manager, and install the ESP8266 manager.  The one we want will be written by the "community".    Prior to compiling, it's necessary to choose board "LOLIN(WEMOS) D1 Mini Lite".  

### VERSIONS ###
* V1.0.0 -- There were a few releases prior to this version.  Just prior to this release, there was an issue in which the garage door button on the HA dashboard would randomly stop working.  When investigating, it was clear the module was attempting to reconnect to the MQTT server, however, the issue was that it was not first attempting to reconnect to the WiFi router -- which it was getting kicked off of.  This version includes provisions to verify a WiFi connection is stable prior to attempting to reconnect to the MQTT server.  Also, the button-push-dwell was increased to 900ms.  

* v2.0.0 -- This revision is quite different than the previous, in that the module now supports configuration.  By strapping pins D6 and D5, the unit can be configured to control one of four different garage door types.  By appropriately strapping said pins, one of the following garage door types can be controlled: Meghan's side; Clinton's side; the West shop door; or the East shop door.  

* v2.0.1 -- This isn't a released version, but significant changes has occurred, thus warranting a snapshot.  The config bits now help to define a unique host name so that each controller on the network has a different host name.  The relay drive signal is currently a PWM by way of utilizing the analogWrite function.    