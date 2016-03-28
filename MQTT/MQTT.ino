#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// rename sample_config.h to config.h
#include "config.h"

// ESP8266-01 GPIO2
#define RELAYPIN 2

byte light1Status;
byte prevLight1Status = 0;
char valueStr[5];

WiFiClient wiFiClient;
PubSubClient client(wiFiClient);

void setup() {
  pinMode(RELAYPIN, OUTPUT);
  Serial.begin(115200);

  delay(100);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  WiFi.printDiag(Serial);

  client.setServer(SERVER, SERVERPORT);
  client.setCallback(callback);
}

void loop() {
  yield();
  if (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("", MQTT_USERNAME, MQTT_KEY)) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe(USERNAME PREAMBLE T_LIGHT1, 1);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
    return;
  }

  if (prevLight1Status != light1Status ) {
    Serial.println("Publish Status");

    String hi = (String)light1Status;
    hi.toCharArray(valueStr, 2);
    client.publish(USERNAME PREAMBLE T_LIGHT1STATUS, valueStr);
    prevLight1Status = light1Status;
  }

  client.loop();
}

void callback(char* topic, byte * data, unsigned int length) {
  Serial.print(topic);
  Serial.print(": ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)data[i]);
  }
  Serial.println();
  if (data[1] == 'F') {
    light1Status = 0;
    digitalWrite(RELAYPIN, LOW);
  } else {
    light1Status = 1;
    digitalWrite(RELAYPIN, HIGH);
  }
}

