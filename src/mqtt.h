

WiFiClient mqtt_wifi_client;
PubSubClient mqtt_client(mqtt_wifi_client);


void callback(char* topic, byte* payload, unsigned int length) {

  int z_value;

  payload[length] = '\0'; // finish off the String

  Serial.print(topic);
  Serial.print(" => ");
  Serial.println((char *)payload);


  if ( !strcmp(topic, RED_FEED) ) {
    z_value = atoi( (char *)payload );
    Serial.print("RED ");
    Serial.println(z_value);
    //relay_power(z_value);
  }


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
  // Loop until we're reconnected
  //while (!mqtt_client.connected()) {

    Serial.print("Attempting MQTT connection...");
    Serial.print(mqtt_server);
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (mqtt_client.connect(clientId.c_str())) {
      Serial.println("connected");

      // Once connected, publish an announcement...
      //my_publish("outTopic", "hello world");

      // ... and resubscribe

      mqtt_client.subscribe(RED_FEED);

    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");

      // Wait 5 seconds before retrying

      delay(5000);
    }

  //} // loop

}
