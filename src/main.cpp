#include "main.h"

#include "led.h"

#include "mqtt.h"

#include "network.h"

// set variables based on device_role
// currently, only 1 feature to set
void setFeaturesFromRole() {
  Serial.print("role is ");
  Serial.println(device_role);

  if (!strcmp(device_role, "FAN") ) {
    // something
  } else {
    // something
  }

  mqtt_port_int = atoi(mqtt_port);

  //pinLED = atoi(ch_pinLED);

  pinButton = atoi(ch_pinButton);

  pinRed = atoi(ch_pinRed);
  pinGrn = atoi(ch_pinGrn);
  pinBlu = atoi(ch_pinBlu);


  pinMode( pinButton, INPUT);

  pinMode( pinLED, OUTPUT);

  pinMode( pinRed, OUTPUT);
  pinMode( pinGrn, OUTPUT);
  pinMode( pinBlu, OUTPUT);

}


void readconfig() {
  //clean FS, for testing
  //SPIFFS.format();

  //read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("parsed json");

          if ( json.containsKey("mqtt_server") ) {
            strcpy(mqtt_server, json["mqtt_server"]);
            Serial.print("server is ");
            Serial.println(mqtt_server);
          }

          if ( json.containsKey("mqtt_port") ) {
            strcpy(mqtt_port, json["mqtt_port"]);
          }

          if ( json.containsKey("device_role") ) {
            strcpy(device_role, json["device_role"]);
            Serial.print("role is ");
            Serial.println(device_role);
          }

          if ( json.containsKey("mqtt_nickname") ) {
            strcpy(mqtt_nickname, json["mqtt_nickname"]);
            Serial.print("nickname is ");
            Serial.println(mqtt_nickname);
          }

          if ( json.containsKey("pinButton") ) {
            strcpy(ch_pinButton, json["pinButton"]);
            Serial.print("Button pin is ");
            Serial.println(ch_pinButton);
          }

          if ( json.containsKey("pinRed") ) {
            strcpy(ch_pinRed, json["pinRed"]);
            Serial.print("Red pin is ");
            Serial.println(ch_pinRed);
          }

          if ( json.containsKey("pinGrn") ) {
            strcpy(ch_pinGrn, json["pinGrn"]);
            Serial.print("Green pin is ");
            Serial.println(ch_pinGrn);
          }

          if ( json.containsKey("pinBlu") ) {
            strcpy(ch_pinBlu, json["pinBlu"]);
            Serial.print("Blue pin is ");
            Serial.println(ch_pinBlu);
          }

          setFeaturesFromRole();

          //strcpy(blynk_token, json["blynk_token"]);

        } else {
          Serial.println("failed to load json config");
        }
      } else {
        Serial.println("failed to open json config");
      }
    } else {
      Serial.println("json config does not exist");
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read
}

void setup() {

  randomSeed(micros());

  Serial.begin(115200);

  if (pinLED >= 0) {
    pinMode( pinLED, OUTPUT);
  }

  ticker_control.attach(0.5, toggle_LED);

  readconfig();

  setup_wifi(); // network.h

  mqtt_client.setServer(mqtt_server, mqtt_port_int); // was 1883
  mqtt_client.setCallback(callback); // callback is in mqtt.h

  //current_relay_power = 0;
  //digitalWrite(pinRelay, RELAY_POWER_OFF);

  mqtt_reconnect(); // establish MQTT for first time
  //mqtt_client.loop();

  ticker_control.detach();

  set_LED( LEDOn ); // turn LED off -- led.h

}


void loop() {

  // button-press logic
  // adapted from http://www.instructables.com/id/Arduino-Dual-Function-Button-Long-PressShort-Press
  // is button down? !read
  if ( !digitalRead(pinButton) ) {

    // button is down
		if ( !buttonActive ) {
			buttonActive = true;
			buttonTimer = millis();
		}

		if ( (millis() - buttonTimer > longPressTime) && !longPressActive ) {
			longPressActive = true;
      // LONG PRESS STARTED
      Serial.println("long press start");
      ticker_control.attach(0.3, toggle_LED);
		}

	} else {
    // button is now up

		if ( buttonActive ) {
      // button WAS down
			if ( longPressActive ) {
				longPressActive = false;
        // LONG PRESS ENDED
        Serial.println("long press end");

        WiFiManager wifiManager;

        //reset saved settings
        wifiManager.resetSettings();

        Serial.println("reset");
        delay(5000);
        ESP.reset();
        delay(2000);
        while (1) {
          // spin until reset
        }

			} else {
        // short press ended
        Serial.println("short press end");
        Serial.println("version F 2017-01-07");


        //if (mqtt_client.connected() ) {
          //char msg[10];
          //snprintf (msg, 10, "%i", current_relay_power);

          //mqtt_client.publish(POWER_FEED, msg);
        //}

			}
			buttonActive = false;

      //digitalWrite(pinLED, 1 - LEDOn);
    }

  } // button was up

  // handle OTA
  ArduinoOTA.handle();

  // reconnect to MQTT if necessary
  if (!mqtt_client.connected()) {
    mqtt_reconnect(); // subscribe happens here
  }

  // handle subscription changes
  mqtt_client.loop();

}
