#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <time.h>

// WiFi credentials
const char* ssid = "KRAKKEN";
const char* password = "Angled-Agility2-Prism";

// MQTT Broker credentials
const int mqtt_port = 8883; // 8883
// const char* mqtt_broker = "10.0.0.157";
const char* mqtt_broker = "prokofiev.bakken.io";
const char* mqtt_topic = "test";
const char* mqtt_username = "guest";
const char* mqtt_password = "guest";

// NTP Server settings
const char *ntp_server = "pool.ntp.org";     // Default NTP server
const long gmt_offset_sec = 0;            // GMT offset in seconds (adjust for your time zone)
const int daylight_offset_sec = 0;        // Daylight saving time offset in seconds

WiFiClientSecure espClient;
PubSubClient mqtt_client(espClient);

// SSL certificate for MQTT broker
const char ca_cert[] = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDhjCCAm6gAwIBAgIUKB4oZ316T1t310FUiGklnC4XeaYwDQYJKoZIhvcNAQEL\n" \
"BQAwTDE7MDkGA1UEAwwyVExTR2VuU2VsZlNpZ25lZHRSb290Q0EgMjAyNC0wNS0w\n" \
"M1QxNTo1MDo1Mi44NTk4NTQxDTALBgNVBAcMBCQkJCQwHhcNMjQwNTAzMjI1MDUy\n" \
"WhcNMzQwNTAxMjI1MDUyWjBMMTswOQYDVQQDDDJUTFNHZW5TZWxmU2lnbmVkdFJv\n" \
"b3RDQSAyMDI0LTA1LTAzVDE1OjUwOjUyLjg1OTg1NDENMAsGA1UEBwwEJCQkJDCC\n" \
"ASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAJRdQRW1NGru4L3LcaFyGb4/\n" \
"VST4dn6WmN593n/c6P8fYciEuRvvR5CN/YbKfx86lNA/C/ceoQ68dq/UWt2hu4we\n" \
"LHb4nfZTKdKWkWr8HuLEgWwn+bILglsTdb3twKnQZI6xRZjyL4ReLAr/jqPnZcRj\n" \
"hhMTW2Mbnc/ROuBEkgZ8Ds1sk7ZAJScQSWL7Q7wKB3HTgQYnvrtqrtA2xqoCFvzQ\n" \
"q3LUlwNq/O3aAq8A07VlllFq1zsBb5BX4dsX6t5+qE4nHyU7auh9DAY92c03ed2z\n" \
"IiP/sLSeRNmcZiDFshNnWvudb3uD4OBor3TpFoKscbQlVWSFwMBQXgkELGK4gNkC\n" \
"AwEAAaNgMF4wDwYDVR0TAQH/BAUwAwEB/zALBgNVHQ8EBAMCAQYwHQYDVR0OBBYE\n" \
"FCcJTnwbY90x15tHjRXIUB7x1cQJMB8GA1UdIwQYMBaAFCcJTnwbY90x15tHjRXI\n" \
"UB7x1cQJMA0GCSqGSIb3DQEBCwUAA4IBAQAdyTcYTnIQq4Oe65X9BSZxgp7+rp67\n" \
"unWMU2+LJAu9FSzdv6wflnsrX/+c2f6V3FTs1KoleanP0BMkjXtVZfglQmF2L4E0\n" \
"/LuA5FjSh+BZ29BnTUL6/pmoiQ1huUj/9cy5NRubupvGk1TLaHKDzmKPK8KIci9O\n" \
"ZZ9+GSWT4aKjRR/cpRMdDT+GQ33vI4xrzfVp359gojlGvaLm14+K5rlT4SQwg+y2\n" \
"xijUMSAG1bEgmK5O8u4So+gPdsb3XZr92hKZFKBrdCIizhB8VzWUBFwXjtH4SCaK\n" \
"TxaRt44Wd/2L6GdKYfLa8VHnUSefu/5GR1XyTw/Ue3sJq8Itgr///Es1\n" \
"-----END CERTIFICATE-----\n";

const char client_certificate[] = \
"-----BEGIN CERTIFICATE-----\n" \
"MIID0DCCArigAwIBAgIBAjANBgkqhkiG9w0BAQsFADBMMTswOQYDVQQDDDJUTFNH\n" \
"ZW5TZWxmU2lnbmVkdFJvb3RDQSAyMDI0LTA1LTAzVDE1OjUwOjUyLjg1OTg1NDEN\n" \
"MAsGA1UEBwwEJCQkJDAeFw0yNDA1MDMyMjUwNTNaFw0zNDA1MDEyMjUwNTNaMC8x\n" \
"HDAaBgNVBAMME3Byb2tvZmlldi5iYWtrZW4uaW8xDzANBgNVBAoMBmNsaWVudDCC\n" \
"ASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALE4RDOoORy267CMWEMLhz3L\n" \
"9d9vVYFBY4xUEM06nGIntX91IJC6vJk8D4QttigXZ790S/iqiMlHUlbtR+6JyZMo\n" \
"Dz1vreSrb4HRyPmznzZ4Yz2fMGywwyoY5oiY2eQQIf74IVvyoy0BlQV6bLJ5hjPa\n" \
"3RJ5w8bfU7VR5URlBQXSpSU7LyULKnO5d8FmiieqJujEHtIPbyINVZkTQNkiACtZ\n" \
"46LdPVcqohZ9oBppBEQeCODKmXHvamLSCImAWHRg9iVqXkx88DABZFU3I+eYqNG5\n" \
"uLIJfQKZSB9V3ZiKMCBar+gxBfaGHb8aLXKLDY87YMkDDYTGpDTjrQOUICd9Io8C\n" \
"AwEAAaOB2TCB1jAJBgNVHRMEAjAAMAsGA1UdDwQEAwIFoDATBgNVHSUEDDAKBggr\n" \
"BgEFBQcDAjA0BgNVHREELTArghNwcm9rb2ZpZXYuYmFra2VuLmlvgglQUk9LT0ZJ\n" \
"RVaCCWxvY2FsaG9zdDAxBgNVHR8EKjAoMCagJKAihiBodHRwOi8vY3JsLXNlcnZl\n" \
"cjo4MDAwL2Jhc2ljLmNybDAdBgNVHQ4EFgQUMzSJ/iW0bTsRZZGQrANG7es7wAUw\n" \
"HwYDVR0jBBgwFoAUJwlOfBtj3THXm0eNFchQHvHVxAkwDQYJKoZIhvcNAQELBQAD\n" \
"ggEBACfhtSbYlA4YApOEVf3mYQCQbxZfIHW/3rlBcgarRCtdM5eROtiyfAksjyKG\n" \
"4zu/phifhbJB/v/IGd4F9pqYT4+5fBl4Czoxy+YNJOfZVb5YMZ2UjBphnASQzIPd\n" \
"9lr6auKm4sBT+TiwgCkYsGRximVaAIkERmxtvYfMRS9wkUht7dgnI1/R0qbsKU9e\n" \
"c4hLuAzIq5NEJgiRsWUJzm8QYq2TDVeUjIhuVeUR53mXR5wYWfsJatVXwUW7Vr/e\n" \
"i4R0E71CBMbJKF8hkWKmFyNSp8c6rnF0YGHAZgdK9Fjb2yeqwqHJY4vLbOkSvZvs\n" \
"Ylw9q1d+WNnCteiMfnyDTEESCAM=\n" \
"-----END CERTIFICATE-----\n";

const char client_key[] = \
"-----BEGIN PRIVATE KEY-----\n" \
"MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCxOEQzqDkctuuw\n" \
"jFhDC4c9y/Xfb1WBQWOMVBDNOpxiJ7V/dSCQuryZPA+ELbYoF2e/dEv4qojJR1JW\n" \
"7UfuicmTKA89b63kq2+B0cj5s582eGM9nzBssMMqGOaImNnkECH++CFb8qMtAZUF\n" \
"emyyeYYz2t0SecPG31O1UeVEZQUF0qUlOy8lCypzuXfBZoonqiboxB7SD28iDVWZ\n" \
"E0DZIgArWeOi3T1XKqIWfaAaaQREHgjgyplx72pi0giJgFh0YPYlal5MfPAwAWRV\n" \
"NyPnmKjRubiyCX0CmUgfVd2YijAgWq/oMQX2hh2/Gi1yiw2PO2DJAw2ExqQ0460D\n" \
"lCAnfSKPAgMBAAECggEAEhmakaIVTzL5iHsVCqfzRKL/tjXTSFPK3Iq7epKJZVcD\n" \
"6QQn8tA3sfjZ5tiaWabmUD4LAlc8iD4yETHD3n+RKGf3goFVA91kkqBABAzN7ozG\n" \
"KgWHtxoonLSVWw8tdecV6coi8haqhs66DyitUF8sD0DDwn7QOZAWpxa5sU4WAL2S\n" \
"v7KGkmt5CkiHedb7CtX5OUgRzNuxSwfS4PnDKzwgwkO5V0LH+VyKQKIfYsxB+Yvi\n" \
"wM183NYvK85VmO5VG1ddJAqCZWdm2/AN7U418nTq0zI1GomhyzSxEt8XqJKKeMSX\n" \
"mxtU0klHWFrKYY6NAoBH4owxojlHFzeB1PaIdRTlBQKBgQDmJERojGEUDlAe2KPd\n" \
"H8ppmXbseyYh6mOudPGdlYPotDaMydYXbhAgt3T8wluGpUOCVklKCGnvAYGdr3EN\n" \
"NTaT0BUZAathVE4mugN2dILCNfxrzamZRe+8zJN4meA/J/2dQlN0tfHkG/K63FNT\n" \
"D4PNRQV7tJMHyqEOUw19WbcofQKBgQDFIcTCPQZvIuYiYBb+bOZ81haseyZWKJH2\n" \
"gmcMf46QYmjKQX4LzVsAwyDipOv6Ct2meCNGpDVMfad8/Ul73rXKrBaqDgjBlCA/\n" \
"QZYJZ+J33NBaxBrhi0SfDkrkv9sucFuugxorx+2QG0efZduv04WwSH7pK6HX3mjm\n" \
"/q/ZYGUw+wKBgCbl+F4HZB0ksiwUh9XHGsowcyZTRkWzPTfuciwlak85roKrVrFN\n" \
"HBK7j11taFTguAFenjKSK32HLqY9UglediQTpo08eAASZMcs54rs9HDo+Qorj7/l\n" \
"TzkFBuP6dimWk8oxvLNcUU8x3hvxQ3AljixyvxOCmNtXR1OJfB00vb1tAoGAYGkn\n" \
"hae64de7ncxI8p3oC93YsPp5wWGOIXWdWGt6GftHLMPjFXqVq0XDa5UlI+D0jlnr\n" \
"Rr1Ao+xAYcyuh7pgRoLrgrfiaGo9azJ0Yp1AEUcZT4rharVmFjQzRLTgSVICRNMx\n" \
"lX/uG7B70uVKEC/vDkwmZXtFaAIKZA7jifivBMkCgYEAsWGMNl//qbIwwXJlyd3m\n" \
"UBh651MAMKu5A6JSVxP7wY3SWF2NNbwwbidIvtt9diB7FK93wHmspCJXphufnYhT\n" \
"hLC8WsQr9y4+aHouJLOHWWHKW4skzww0MCVxasdm7vEFcjEuJdKrwnO8MiBbqsFh\n" \
"/l+CvHjXOsO8Z4SYNeWIL4c=\n" \
"-----END PRIVATE KEY-----\n";

// Function declarations
void connectToWiFi();

void connectToMQTT();

void syncTime();

void mqttCallback(char *topic, byte *payload, unsigned int length);

void setup() {
    Serial.begin(115200);
    connectToWiFi();
    syncTime();  // X.509 validation requires synchronization time
    mqtt_client.setServer(mqtt_broker, mqtt_port);
    mqtt_client.setCallback(mqttCallback);
    connectToMQTT();
}

void connectToWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("[INFO] connecting to WiFi...");
    }
    Serial.println("[INFO] connected to the WiFi network, IP address:");
    Serial.println(WiFi.localIP());
    Serial.println("[INFO] DNS ip address:");
    Serial.println(WiFi.dnsIP());
}

void syncTime() {
    configTime(gmt_offset_sec, daylight_offset_sec, ntp_server);
    Serial.print("Waiting for NTP time sync: ");
    while (time(nullptr) < 8 * 3600 * 2) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("Time synchronized");
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        Serial.print("Current time: ");
        Serial.println(asctime(&timeinfo));
    } else {
        Serial.println("Failed to obtain local time");
    }
}

/*
//Connecting to a mqtt broker width ssl certification
// espClient.setInsecure();
// espClient.setCertificate(ESP_CA_cert);  // for mqtt_client verification
// espClient.setPrivateKey(ESP_RSA_key);    // for mqtt_client verification
*/

void connectToMQTT() {
    BearSSL::X509List serverTrustedCA(ca_cert);
    espClient.setTrustAnchors(&serverTrustedCA);
    // espClient.setInsecure();
    while (!mqtt_client.connected()) {
        String client_id = "esp8266-client-" + String(WiFi.macAddress());
        Serial.printf("[INFO] connecting to MQTT Broker as %s.....\n", client_id.c_str());
        if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("[INFO] connected to MQTT broker");
            mqtt_client.subscribe(mqtt_topic);
            // Publish message upon successful connection
            mqtt_client.publish(mqtt_topic, "Hi RabbitMQ I'm ESP8266 ^^");
        } else {
            char err_buf[128];
            espClient.getLastSSLError(err_buf, sizeof(err_buf));
            Serial.print("[ERROR] failed to connect to MQTT broker, rc=");
            Serial.println(mqtt_client.state());
            Serial.print("[ERROR] SSL error: ");
            Serial.println(err_buf);
            delay(5000);
        }
    }
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {
    Serial.print("[INFO] message received on topic: ");
    Serial.println(topic);
    Serial.print("[INFO] message: ");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
}

/*
void loop() {
  mqtt_client.loop();
  mqtt_client.publish(topic, "message sent from the esp32 to MQTT BROKER encrypted");
  // Send a message every 10 seconds
  delay(10000);
}
*/

void loop() {
    if (!mqtt_client.connected()) {
        connectToMQTT();
    }
    mqtt_client.loop();
}
