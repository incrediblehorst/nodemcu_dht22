#include <ESP8266WiFi.h>
#include "DHT.h"

#define DHTPIN D2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

void setup() {

  const char* ssid     = "NAME_WLAN"; 		//edit W-LAN name
  const char* password = "PASSWORD_WLAN"; 	//edit W-LAN password

  const char* host = "emoncms.org";
  const char* apikey = "API_KEY"; 		//edit API key from emoncms

  Serial.begin(115200);

  dht.begin();
  delay(2000);
  Serial.println();
  Serial.println();
  float h = dht.readHumidity();
  float t = dht.readTemperature();

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

  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");

  ESP.deepSleep(1200e6); 		//edit time intervall (optional)
}

void loop() {

}
