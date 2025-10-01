#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h"

/******** Wi-Fi ********/
#define WLAN_SSID   ""
#define WLAN_PASS   ""

/******** Adafruit IO ********/
#define AIO_SERVER      ""
#define AIO_SERVERPORT  
#define AIO_USERNAME    ""
#define AIO_KEY         ""

/******** MQTT ********/
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/******** Feeds ********/
Adafruit_MQTT_Publish pubAlarma = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "");
Adafruit_MQTT_Publish pubTemp   = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "");
Adafruit_MQTT_Publish pubHum    = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "");

/******** Sensor FC-51 ********/
const int PIN_FC51 = 14;

/******** Sensor DHT11 ********/
#define DHTPIN 25     // Pin de datos del DHT11
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

/******** Funciones ********/
void conectarWiFi() {
  Serial.print("Conectando a WiFi...");
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 40) {
    delay(500); Serial.print(".");
    intentos++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado");
    Serial.print("IP asignada: "); Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nNo se pudo conectar a WiFi");
  }
}

void MQTT_connect() {
  if (mqtt.connected()) return;
  Serial.print("Conectando a Adafruit IO... ");
  int8_t ret; uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Reintento en 10 s...");
    mqtt.disconnect(); delay(10000);
    if (--retries == 0) while (1) delay(1);
  }
  Serial.println("¡Conectado!");
}

/******** Setup ********/
void setup() {
  Serial.begin(115200);
  pinMode(PIN_FC51, INPUT);
  dht.begin();      
  conectarWiFi();
}

/******** Loop ********/
void loop() {
  MQTT_connect();
  mqtt.processPackets(100);
  if (!mqtt.ping()) mqtt.disconnect();

  /***** Sensor FC-51 *****/
  int estado = digitalRead(PIN_FC51);
  bool obstaculo = (estado == LOW);  // LOW = objeto detectado

  if (obstaculo) {
    Serial.println("Objeto detectado");
    pubAlarma.publish("Objeto detectado");
  } else {
    Serial.println("Libre");
    pubAlarma.publish("Libre");
  }

  /***** Sensor DHT11 *****/
  float t = dht.readTemperature(); 
  float h = dht.readHumidity();    

  if (isnan(t) || isnan(h)) {
    Serial.println("Error leyendo DHT11");
  } else {
    Serial.printf("Temp: %.1f °C | Hum: %.1f %%\n", t, h);

    if (!pubTemp.publish(t)) Serial.println("Error publicando temperatura");
    if (!pubHum.publish(h))  Serial.println("Error publicando humedad");
  }

  delay(5000); // Publica cada 5 segundos (Adafruit IO permite intervalos >2s)
}
