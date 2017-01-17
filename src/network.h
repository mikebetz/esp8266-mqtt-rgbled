void startOTA() {
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();
}

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}



void setup_wifi() {

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //reset saved settings
  //wifiManager.resetSettings();

  //set custom ip for portal
  //wifiManager.setAPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  wifiManager.setDebugOutput(true);

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
  WiFiManagerParameter custom_device_role("role", "role FAN or SWITCH", device_role, 10);
  WiFiManagerParameter custom_mqtt_nickname("nickname", "mqtt nickname", mqtt_nickname, 20);

  WiFiManagerParameter custom_pinLED("LEDpin", "LED pin 2", ch_pinLED, 3);
  WiFiManagerParameter custom_pinButton("ButtonPin", "Button pin 0", ch_pinButton, 3);

  WiFiManagerParameter custom_pinRed("Redpin", "Red pin 12", ch_pinRed, 3);
  WiFiManagerParameter custom_pinGrn("Grnpin", "Grn pin 13", ch_pinGrn, 3);
  WiFiManagerParameter custom_pinBlu("Blupin", "Blu pin 14", ch_pinBlu, 3);

  //add all your parameters here
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_device_role);
  wifiManager.addParameter(&custom_mqtt_nickname);

  wifiManager.addParameter(&custom_pinLED);
  wifiManager.addParameter(&custom_pinButton);

  wifiManager.addParameter(&custom_pinRed);
  wifiManager.addParameter(&custom_pinGrn);
  wifiManager.addParameter(&custom_pinBlu);

  //reset settings - for testing
  //wifiManager.resetSettings();

  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  //wifiManager.autoConnect("AutoConnectAP","antwerp");
  //or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();

  if (!wifiManager.autoConnect()) {
    Serial.println("failed to connect, reset");
    delay(5000);
    ESP.reset();
    delay(2000);
    while (1) {
      // spin until reset
    };
  }

  Serial.println("local ip");
  Serial.println(WiFi.localIP());

  /*
  // old code that grabbed the MAC address
  char justabyte[3];
  byte mac[6];
  char *a;
  WiFi.macAddress(mac);

  int i;

  a = &macaddr[0];

  for (i = 0; i<6; i++) {
    sprintf(a, "%02X", mac[i]);
    a++;
    a++;
    *a++ = ':';
  }

  a--;
  *a = 0;

  Serial.print("MAC: ");
  Serial.println(macaddr);
  */

  //read updated parameters
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(mqtt_port, custom_mqtt_port.getValue());
  strcpy(device_role, custom_device_role.getValue());
  strcpy(mqtt_nickname, custom_mqtt_nickname.getValue());

  strcpy(ch_pinLED, custom_pinLED.getValue());
  strcpy(ch_pinButton, custom_pinButton.getValue());

  strcpy(ch_pinRed, custom_pinRed.getValue());
  strcpy(ch_pinGrn, custom_pinGrn.getValue());
  strcpy(ch_pinBlu, custom_pinBlu.getValue());

  Serial.print("server is now ");
  Serial.println(mqtt_server);

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["mqtt_server"] = mqtt_server;
    json["mqtt_port"] = mqtt_port;
    json["device_role"] = device_role;
    json["mqtt_nickname"] = mqtt_nickname;

    json["pinLED"] = ch_pinLED;
    json["pinButton"] = ch_pinButton;

    json["pinRed"] = ch_pinRed;
    json["pinGrn"] = ch_pinGrn;
    json["pinBlu"] = ch_pinBlu;

  ////    json["blynk_token"] = blynk_token;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }

// finish naming the mqtt topics
// NICKNAME-whatever

  strcpy(RED_FEED, mqtt_nickname);
  strcat(RED_FEED, "-");
  strcat(RED_FEED, RED_FEED_SUFFIX);

  strcpy(GRN_FEED, mqtt_nickname);
  strcat(GRN_FEED, "-");
  strcat(GRN_FEED, GRN_FEED_SUFFIX);

  strcpy(BLU_FEED, mqtt_nickname);
  strcat(BLU_FEED, "-");
  strcat(BLU_FEED, BLU_FEED_SUFFIX);

  strcpy(HUE_FEED, mqtt_nickname);
  strcat(HUE_FEED, "-");
  strcat(HUE_FEED, HUE_FEED_SUFFIX);

  strcpy(SAT_FEED, mqtt_nickname);
  strcat(SAT_FEED, "-");
  strcat(SAT_FEED, SAT_FEED_SUFFIX);

  strcpy(VAL_FEED, mqtt_nickname);
  strcat(VAL_FEED, "-");
  strcat(VAL_FEED, VAL_FEED_SUFFIX);

  strcpy(POWER_FEED, mqtt_nickname);
  strcat(POWER_FEED, "-");
  strcat(POWER_FEED, POWER_FEED_SUFFIX);

  setFeaturesFromRole();

  startOTA();

}
