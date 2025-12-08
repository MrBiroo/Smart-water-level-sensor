7#include <WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "";
const char* password = "";

// WiFi credentials
const char* ssid2 = "";
const char* password2 = "";

// MQTT broker info
const char* mqtt_server = "broker.hivemq.com"; // Public broker   <<test.mosquitto.org>>
const int mqtt_port = 1883;


// Ultrasonic pins
#define TRIG_PIN 5
#define ECHO_PIN 18

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;

void setup_wifi() {
  delay(10);
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
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("284d3ca7-9c18-453e-a4b4-58b7b70814ff")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

float measureOnce() {
  static float lastValidDistance = 0;
  float distance;

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, LOW);
  
  
  float duration = pulseIn(ECHO_PIN, HIGH, 15000); // 15ms timeout (~2.5m max range)
  if (duration == 0) {
    // no valid reading
    distance = lastValidDistance; // keep previous value
  } else {
    distance = duration * 0.034 / 2;
    lastValidDistance = distance;
  }

  return distance;
}

float getDistance() {
  float readings[5];
  for (int i=0; i<5; i++) {
    readings[i] = measureOnce(); // your function
    delay(100);
  }
  // sort and return median
  bubbleSort(readings, 5);
  float avr=(readings[2]+readings[3])/2;
  return avr;
}

void checkWiFiWithSleep() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected! Trying to reconnect...");

    WiFi.disconnect();
    WiFi.begin(ssid, password);

    unsigned long startAttemptTime = millis();
    bool connected = false;

    while (millis() - startAttemptTime < 300000) { // 5 min = 300,000 ms
      if (WiFi.status() == WL_CONNECTED) {
        connected = true;
        Serial.println("WiFi reconnected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        break;
      }
      delay(500); // wait half second before next check
      Serial.print(".");
    }

    if (!connected) {
      Serial.println("\nFailed to reconnect WiFi after 5 minutes.");
      Serial.println("Going to deep sleep for 5 minutes...");

      // Configure ESP32 to wake up after 5 min
      esp_sleep_enable_timer_wakeup(5 * 60 * 1000000ULL); 
      esp_deep_sleep_start(); 
    }
  }
}

void bubbleSort(float arr[], int n) {
  for (int i = 0; i < n-1; i++) {
    for (int j = 0; j < n-i-1; j++) {
      if (arr[j] > arr[j+1]) {
        float temp = arr[j];
        arr[j] = arr[j+1];
        arr[j+1] = temp;
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  long interval = 10000; // 15 minutes
  if (now - lastMsg > interval) {
    lastMsg = now;
    float distance = getDistance();
    Serial.print("Distance: ");
    Serial.println(distance);

    float Tank_level = 106 - distance ;
    

    float percent = (Tank_level / 105.0) * 100.0; // persentage of tank level
   
   float area = 67.5 * 133; // cm²
   float liters = (Tank_level * area) / 1000.0; // L

    char msg[50];
    dtostrf(percent, 1, 2, msg);
    client.publish("roof/waterlevel%", msg);

    dtostrf(liters, 1, 2, msg);
    client.publish("roof/waterlevel_liters", msg);

    dtostrf(distance, 1, 2, msg);
    client.publish("roof/waterlevel_distant", msg);    
  }
  
}
