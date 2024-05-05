#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <time.h>

// WiFi credentials
const char* ssid = "KRAKKEN";
const char* password = "Angled-Agility2-Prism";

// MQTT Broker credentials
const int mqtt_port = 8883; // 8883
const char* mqtt_broker = "shostakovich-internal.bakken.io";
const char* mqtt_topic = "test";
const char* mqtt_username = "guest";
const char* mqtt_password = "guest";

// NTP Server settings
const char *ntp_server = "pool.ntp.org";  // Default NTP server
const long gmt_offset_sec = -28800;       // GMT offset in seconds (adjust for your time zone)
const int daylight_offset_sec = 3600;     // Daylight saving time offset in seconds

WiFiClientSecure espClient;
PubSubClient mqtt_client(espClient);

// SSL certificate for MQTT broker
const char ca_certificate[] = \
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
"MIID6DCCAtCgAwIBAgIBBDANBgkqhkiG9w0BAQsFADBMMTswOQYDVQQDDDJUTFNH\n" \
"ZW5TZWxmU2lnbmVkdFJvb3RDQSAyMDI0LTA1LTAzVDE1OjUwOjUyLjg1OTg1NDEN\n" \
"MAsGA1UEBwwEJCQkJDAeFw0yNDA1MDUxNDIyNDVaFw0zNDA1MDMxNDIyNDVaMDsx\n" \
"KDAmBgNVBAMMH3Nob3N0YWtvdmljaC1pbnRlcm5hbC5iYWtrZW4uaW8xDzANBgNV\n" \
"BAoMBmNsaWVudDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMcjP3c7\n" \
"nK5YIbe87SNROMFStHajBKXkDRyZBtRe1WRmeLOetCsMsq1inWS7bgBaMwPYk3jj\n" \
"lKXBVOsQ7qMpMipZaNmAW/9/AzowXLfk2Glw7EkAhPwc9xpvA/1reZbxJQxGU/UT\n" \
"Pwjpdc0q4CCvgOkw70ssPSIRtyrbJk15zevFY6WTzuhpwWssjPpvIioFIENKX/y3\n" \
"uaL7mzxUnCH3WgfZ3AO8h/NAKklruSvZmOAmreSX1cmp8R0Zs92GGe2nHNm+jOB6\n" \
"dPc21oZPHa4GBiYMU0E4rvWXiWS1PGyDjBrNpULpfcdq6y3UEGzi84650G0pczIB\n" \
"75Vgo0WdbvQ92PkCAwEAAaOB5TCB4jAJBgNVHRMEAjAAMAsGA1UdDwQEAwIFoDAT\n" \
"BgNVHSUEDDAKBggrBgEFBQcDAjBABgNVHREEOTA3gh9zaG9zdGFrb3ZpY2gtaW50\n" \
"ZXJuYWwuYmFra2VuLmlvgglQUk9LT0ZJRVaCCWxvY2FsaG9zdDAxBgNVHR8EKjAo\n" \
"MCagJKAihiBodHRwOi8vY3JsLXNlcnZlcjo4MDAwL2Jhc2ljLmNybDAdBgNVHQ4E\n" \
"FgQUQfmH+6UnqJ5nIQeAsgPBIeEGLj8wHwYDVR0jBBgwFoAUJwlOfBtj3THXm0eN\n" \
"FchQHvHVxAkwDQYJKoZIhvcNAQELBQADggEBAEkKgJpbBw9VIqHTn5csZRnVBPSV\n" \
"UKVOOTS0g+wgwgcQiBaOAlW/nPluTPrnIKWv2/lGlK1vP8r+ruzj6g9KxkEDWlww\n" \
"dFfyGXY7Kvf5tlbZ4itrEW7O3UnsQrcwOLLyAs7NRo9rQNYt2IsZjBbGu+PF0ajf\n" \
"lJwTq8b3hWGBry4IN/gOyX4tStnUTNVpugFdUoHPRpRWKI5mf5UtMbG/m6GoHNiS\n" \
"H/8BAkh1pufbPwXL/BQWqFeBEfJAQXu4o4X8p9bjj7mSxFxx1WdB2qtLAXYBzdC2\n" \
"FrsMbcJbvgUXL1YSWYJdm591NHJu6rTzGhl+Led3d2Sl5oVAKTd9Nqx1RSY=\n" \
"-----END CERTIFICATE-----\n";

const char client_key[] = \
"-----BEGIN PRIVATE KEY-----\n" \
"MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQDHIz93O5yuWCG3\n" \
"vO0jUTjBUrR2owSl5A0cmQbUXtVkZniznrQrDLKtYp1ku24AWjMD2JN445SlwVTr\n" \
"EO6jKTIqWWjZgFv/fwM6MFy35NhpcOxJAIT8HPcabwP9a3mW8SUMRlP1Ez8I6XXN\n" \
"KuAgr4DpMO9LLD0iEbcq2yZNec3rxWOlk87oacFrLIz6byIqBSBDSl/8t7mi+5s8\n" \
"VJwh91oH2dwDvIfzQCpJa7kr2ZjgJq3kl9XJqfEdGbPdhhntpxzZvozgenT3NtaG\n" \
"Tx2uBgYmDFNBOK71l4lktTxsg4wazaVC6X3Haust1BBs4vOOudBtKXMyAe+VYKNF\n" \
"nW70Pdj5AgMBAAECggEACYStSWkvhYPP5MTiftAoitEFsC/9jmJWoBnOYkNAJuLg\n" \
"/RDmYrJiLfhrNn/g86JcEuB8uAgEuGGDyhKOnXzgGXcZlF/WxeEPye262M1W7L9J\n" \
"g/nC4XwO8ZQB2FCVSLow/6Oy6qFnX2ktSMe2W8mXu02r+mdcaq73FtTPwV9DkPPS\n" \
"R//9boc0bMrjED89mPDp1JsnuOHkVVxFYBhlmLmIWPHW7Y9lsLMfmdbKKnRWs4n9\n" \
"UQU2kszTqTROMt2DXO37mc2UAbLktRWEiGAGbukaJVbyGxuROmeL+BZPY9noDwoK\n" \
"6KHpUGnNfGEVMYHoZNMr5qG0WDU6IuiW5SutaKtyawKBgQDkjaLyB2CnQRGMEzHY\n" \
"Cn9Zm/cFvEu6ZSsHwuG8hR0voTCeck6vhM8tdWNChNAC7dbci46oy5L6JnTWzfjb\n" \
"CV1FJ1TANavdwBGynnfMt7SqQQxfgIs79Zo0erS0FOezQw1qkxdwmsuFPxzxI/QI\n" \
"5iQ7u98ISltwqcF9ct9benzi9wKBgQDfDUtrHjHYavf/IqC4JkOOtkhAi+JwKcir\n" \
"on4ivSnXKsh4LBsUXjGLmlhOq2jIAywTTFNKqD83hdOy//dN+Xev6jdPR7mYZvnb\n" \
"oS+icjVStJIpHSy/X4sNG9SwiYSQJ2HsldjWwrLwPDmMLLB81/Acx2NtUqlzWGsl\n" \
"hBPMRAg3jwKBgCsoD4eyGu8aG00e5r+Lo+Zwy+E6X/ZPWiO6CdQGMq+muT6CCRed\n" \
"dV15GAgcegbDkFnOxF5dKmCv2EojVIVPrFv9hERTHiPeO7lFsQtn+OZZKBP0etYU\n" \
"l0yniu2iMOoplCP2qGtr31hGxZlHF7KCen8JB6FSIRgSBeZBRanJZEGbAoGARHJg\n" \
"9WMTqrTQm3N13GypiI65R01l1EOON9UqXeCsiUGwuOfX+30YOR3JAYMqKU6H5Pgk\n" \
"CyHsHE6t8x0HN99Y8huuk16QPotDGzmwU2o2oMWWoJU6WROpqQEkWHQqwM3FCAPH\n" \
"wceHf58Ljc9c7UKahUpOYYrfQu0cHCwWozUxTFMCgYEA2KQu/X+3pt5KRUBs/I1F\n" \
"wjno+vpzNTbZX+xXDm3C9Perk07Zg3W5JKC3b1HzwLNb9K1O6MCG387ejvxcmlun\n" \
"4jCLKKxHys/b4q1QoBFb0VXmAlE2HDQ2CfX8vaCmviwVSo7CRAyRJOQm2jy76LO5\n" \
"npYOmoOCasbanYfuVhRj/eA=\n" \
"-----END PRIVATE KEY-----\n";

BearSSL::X509List serverTrustedCA(ca_certificate);
BearSSL::X509List clientRsaCert(client_certificate);
BearSSL::PrivateKey clientPrivateKey(client_key);

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

void connectToMQTT() {
    espClient.setTrustAnchors(&serverTrustedCA);
    espClient.setClientRSACert(&clientRsaCert, &clientPrivateKey);
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

void loop() {
    if (!mqtt_client.connected()) {
        connectToMQTT();
    }
    mqtt_client.loop();
}
