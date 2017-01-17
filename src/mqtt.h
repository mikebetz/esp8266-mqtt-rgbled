

WiFiClient mqtt_wifi_client;
PubSubClient mqtt_client(mqtt_wifi_client);


void callback(char* topic, byte* payload, unsigned int length) {

  float z_value;

  toggle_LED(); // start the blink

  payload[length] = '\0'; // finish off the String

  Serial.print(topic);
  Serial.print(" => ");
  Serial.println((char *)payload);


  if ( !strcmp(topic, RED_FEED) ) {
    z_value = atof( (char *)payload );
    Serial.print("RED ");
    Serial.println(z_value);

    currentRed = z_value;
    if (pinRed >= 0) {
      analogWrite(pinRed, map(currentRed * currentPower, 0, MAX_RGB_VALUE, 0, PWMRANGE));
    }
  }

  if ( !strcmp(topic, GRN_FEED) ) {
    z_value = atof( (char *)payload );
    Serial.print("GRN ");

    Serial.println(z_value);
    currentGrn = z_value;

    if (pinGrn >= 0) {
      analogWrite(pinGrn, map(currentGrn * currentPower, 0, MAX_RGB_VALUE, 0, PWMRANGE));
    }
  }

  if ( !strcmp(topic, BLU_FEED) ) {
    z_value = atof( (char *)payload );
    Serial.print("BLU ");

    Serial.println(z_value);
    currentBlu = z_value;

    if (pinBlu >= 0) {
      analogWrite(pinBlu, map(currentBlu * currentPower, 0, MAX_RGB_VALUE, 0, PWMRANGE));
    }
  }

  if ( !strcmp(topic, POWER_FEED) ) {
    z_value = atoi( (char *)payload );
    Serial.print("POWER ");
    Serial.println(z_value);

    currentPower = z_value;

    Serial.println(pinRed);
    if (pinRed >= 0) {
      // PWMRANGE is typically 1023 ... not sure where it is defined
      Serial.println(map(currentRed * currentPower, 0, MAX_RGB_VALUE, 0, PWMRANGE));
      analogWrite(pinRed, map(currentRed * currentPower, 0, MAX_RGB_VALUE, 0, PWMRANGE));
    }

    Serial.println(pinGrn);
    if (pinGrn >= 0) {
      Serial.println(map(currentGrn * currentPower, 0, MAX_RGB_VALUE, 0, PWMRANGE));
      analogWrite(pinGrn, map(currentGrn * currentPower, 0, MAX_RGB_VALUE, 0, PWMRANGE));
    }

    Serial.println(pinBlu);
    if (pinBlu >= 0) {
      Serial.println(map(currentBlu * currentPower, 0, MAX_RGB_VALUE, 0, PWMRANGE));
      analogWrite(pinBlu, map(currentBlu * currentPower, 0, MAX_RGB_VALUE, 0, PWMRANGE));
    }

    //relay_power(z_value);
  }


  if ( !strcmp(topic, HUE_FEED) ) {
    z_value = atoi( (char *)payload );
    Serial.print("HUE ");
    Serial.println(z_value);
    //relay_power(z_value);
  }

  if ( !strcmp(topic, SAT_FEED) ) {
    z_value = atoi( (char *)payload );
    Serial.print("SAT ");
    Serial.println(z_value);
    //relay_power(z_value);
  }

  if ( !strcmp(topic, VAL_FEED) ) {
    z_value = atoi( (char *)payload );
    Serial.print("VAL ");
    Serial.println(z_value);
    //relay_power(z_value);
  }

toggle_LED(); // complete the blink

// show power status
set_LED(1 - currentPower); // LED is inverted. power = 1 needs 0 to turn on LED

}

boolean my_publish(const char* topic, char* value) {
  Serial.print("pub: ");
  Serial.print(value);
  Serial.print(" => ");
  Serial.println(topic);
  if (!mqtt_client.connected() ) {
    Serial.println("mqtt not connected");
    return false;
  }

  return mqtt_client.publish(topic, value, true); // true = retained

}

void mqtt_reconnect() {

  if ( mqtt_client.connected() ) {
    return;
  }
  // Loop until we're reconnected
  //while (!mqtt_client.connected()) {

    Serial.print("Attempting MQTT connection...");
    Serial.print(mqtt_server);
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (mqtt_client.connect(clientId.c_str())) {
      Serial.print("connected as");
      Serial.println(clientId.c_str());

      // Once connected, publish an announcement...
      //my_publish("outTopic", "hello world");

      // ... and resubscribe

      Serial.print("Subscribe ");
      Serial.println(RED_FEED);
      mqtt_client.subscribe(RED_FEED);

      Serial.print("Subscribe ");
      Serial.println(GRN_FEED);
      mqtt_client.subscribe(GRN_FEED);

      Serial.print("Subscribe ");
      Serial.println(BLU_FEED);
      mqtt_client.subscribe(BLU_FEED);

      //mqtt_client.subscribe(HUE_FEED);
      //mqtt_client.subscribe(SAT_FEED);
      //mqtt_client.subscribe(VAL_FEED);

      Serial.print("Subscribe ");
      Serial.println(POWER_FEED);
      mqtt_client.subscribe(POWER_FEED);

    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");

      // Wait 5 seconds before retrying

      delay(5000);
    }

  //} // loop

}
