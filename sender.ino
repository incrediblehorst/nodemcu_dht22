#include <ESP8266WiFi.h>
#include "DHT.h"

#define DHTPIN D2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

void setup() {

  const char* ssid     = "NAME_WLAN";
  const char* password = "PASSWORD_WLAN";

  const char* host = "emoncms.org";
  const char* apikey = "API_KEY";

  Serial.begin(115200);

  dht.begin();
  // Wait a few seconds between measurements.
  delay(2000);
  Serial.println();
  Serial.println();
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    ESP.deepSleep(10e6);
  }


  Serial.println(h);
  Serial.println(t);

  Serial.println();
  Serial.println();
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


  delay(5000);

  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    ESP.deepSleep(60e6);
  }

  String temp = String(t);
  String feucht = String(h);

  String url = "/emoncms/input/post.json?node=Speicher&json={Temperatur:" + temp + ",Feuchtigkeit:" + feucht + "}&apikey=" + apikey;

  Serial.print("Requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(2000);

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");

  ESP.deepSleep(1200e6);
}

void loop() {

}
