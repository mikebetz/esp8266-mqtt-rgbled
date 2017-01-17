# esp8266-mqtt-rgbled
mqtt-based controller for RGB lights (non-addressable)

I want to be able to set the hue/saturation/brightness (actually, red/green/blue) of a string of lights
This project helps to do this.

There is also a HAPNode.js accessory

and a Node-Red flow involved

The accessory uses mqtt topics LED01-Hue, LED01-Saturation, LED01-Brightness and LED01-Power

The flow translates the values in the LED01-Hue, LED01-Saturation, LED01-Brightness topics to the LED01-Red, LED01-Green, LED01-Blue topics.

The ESP8266 code watches the LED01-Red, LED01-Green, LED01-Blue and LED01-Power topics and sets the color of the LED strip.

A long (20seconds) button press (ground) on GPIO 0 will start up the WifiManager code ... forcing a re-entry of the wifi credentials but also allowing changing the broker server information etc.

