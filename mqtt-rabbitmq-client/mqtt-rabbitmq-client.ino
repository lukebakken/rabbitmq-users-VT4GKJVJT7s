#include "ESP8266WiFi.h"
#include <PubSubClient.h>
#include <time.h>

// WiFi credentials
const char* ssid = "KRAKKEN";
const char* password = "Angled-Agility2-Prism";

// MQTT Broker credentials

const char* mqtt_broker = "10.0.0.157";
const char* topic = "test";
const char* mqtt_username = "guest";
const char* mqtt_password = "guest";
const int mqtt_port = 1883; // 8883

WiFiClient espClient;
PubSubClient client(espClient);

time_t now;
time_t nowish = 1510592825;

void NTPConnect(void) {
  Serial.print("[INFO] setting time using SNTP...");
  configTime(1 * 3600, 0 * 3600, "pool.ntp.org", "time.nist.gov");
  now = time(nullptr);
  while (now < nowish) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("done!");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

void setup() {
  // Set software serial baud to 115200;
  Serial.begin(115200);
  Serial.println();

  // Connecting to a WiFi network
  WiFi.begin(ssid, password);

  Serial.println("[INFO] begin connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("[INFO] connected to the WiFi network, IP address:");
  Serial.println(WiFi.localIP());

  NTPConnect();
  
  /*
  //Connecting to a mqtt broker width ssl certification
  // espClient.setCACert(CA_cert);
  // espClient.setInsecure();
  // espClient.setCertificate(ESP_CA_cert);  // for client verification
  /// espClient.setPrivateKey(ESP_RSA_key);    // for client verification
  */

  // Connect to the MQTT Broker remotely
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  while (!client.connected()) {
    String client_id = "esp32-client";
    client_id += String(WiFi.macAddress());

    // Print the Name and the id of the esp32 controller
    Serial.printf("[INFO] the client %s connects to the mqtt broker\n", client_id.c_str());

    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("[INFO] mqtt broker connected");
    } else {
      Serial.print("[INFO] connection failed with state:");
      Serial.println(client.state());
      delay(2000);
    }
  }
  // Publish and Subscribe
  client.publish(topic, "Hi I'm ESP32 ^^");
  client.subscribe(topic);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("[INFO] message arrived in topic:");
  Serial.println(topic);
  Serial.print("[INFO] message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}

void loop() {
  client.loop();
  client.publish(topic, "message sent from the esp32 to MQTT BROKER encrypted");
  // Send a message every 10 seconds
  delay(10000);
}