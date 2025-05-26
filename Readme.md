# WEMOS Garage Door Assistant #

This repository contains the source file for the WEMOS D1 Mini (lite) whose job is to 'push' the garage door open/close button upon receiving the proper payload associated with the appropriate garage door topic (MQTT).  This device allows for garage door activity to be automated within HA.  Automations within HA are simply required to publish the appropriate payload to the appropriate topic.  Within homeassistant, there will exist a cover for the garage door.  Total functionality requires the state of the door -- i.e. either open or close -- to be known.  Knowing the state of the door is accomplished via a garage door tilt sensor, in which an automation within homeassistant publishes the current state to the appropriate garage door topic. 

Starting with version __2.0.0__, the unit can be configured to control one of four different garage door types.  The following table describes how configuration is accomplished by properly strapping ID pins D5 and D6.    

Bits (D6:D5)    | Garage Door
:-------------: | :--------------: 
00              | Single Garage Door
01              | Double Garage Door 
10              | Shop Door West
11              | Shop Door East


### TOOL SUITE ###

The source file(s) are to be compiled using Arduino IDE v1.8.8, or newer.    

It is necessary to include esp8266 libraries by including the URL to the repo.  Navigate to File > Preferences, and add the following URL: http://arduino.esp8266.com/stable/package_esp8266com_index.json

After pointing to esp8266 libraries, it is necessary to go to `tools > board > boards manager`, and install the ESP8266 manager.  The one we want will be written by the "community".    Prior to compiling, it's necessary to choose board *LOLIN(WEMOS) D1 Mini Lite*.  

It is imperative to install *PubSubClient* which was written by Nick O'Leary.  Do this by first going to `Sketch > Include Library > Manage Libraries`.  Then search for an install the aforementioned library.

Lastly, it is necessary to install the Silicon Labs CP210x driver.  

### VERSIONS ###
* V1.0.0 -- There were a few releases prior to this version.  Just prior to this release, there was an issue in which the garage door button on the HA dashboard would randomly stop working.  When investigating, it was clear the module was attempting to reconnect to the MQTT server, however, the issue was that it was not first attempting to reconnect to the WiFi router -- which it was getting kicked off of.  This version includes provisions to verify a WiFi connection is stable prior to attempting to reconnect to the MQTT server.  Also, the button-push-dwell was increased to 900ms.  

* v2.0.0 -- This revision is quite different than the previous, in that the module now supports configuration.  By strapping pins D6 and D5, the unit can be configured to control one of four different garage door types.  By appropriately strapping said pins, one of the following garage door types can be controlled: Meghan's side; Clinton's side; the West shop door; or the East shop door.  

* v2.0.1 -- This isn't a released version, but significant changes have occurred, thus warranting a snapshot.  The config bits now help to define a unique host name so that each controller on the network has a different host name.  The relay drive signal is currently a PWM by way of utilizing the analogWrite function.  

* v2.1.0 -- This version is intended to run on the newer controller which implements relay control via a charge pump circuit.  Additionally, upon reset of the controller, jumpers positions are read and the appropriate button topic gets assigned to char buffer __assigned_button_topic__.  This produces cleaner code in the callback routine, as there's only one if statement that checks to see if the new/incoming MQTT topic matches the __assigned_button_topic__.

* v3.0.0 -- This version was tagged on 5/26/25.  The impetus for the change was the new security system running on the RPi5.  Specifically, the username (for MQTT) was updated to cjg_mqtt, as well the password getting updated.  The SW version is now printed out as a debug message.  If we look at the serial log, we can clearly see that the button is being pushed, but the garage door isn't moving.  Although much was fixed in this version, there is still some work to do here.  