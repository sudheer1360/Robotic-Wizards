#define BLYNK_TEMPLATE_ID "TMPL3xRODO13E"
#define BLYNK_TEMPLATE_NAME "Synus"
#define BLYNK_AUTH_TOKEN "DAL1tnHgxNT-XvzAkHT-VPTDvVpCz1R8"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <TinyGPSPlus.h>

const int soilMoisturePin = 34;
const int relayPin = 25;
const int irSensorPin = 26;

TinyGPSPlus gps;
BlynkTimer timer;

int moistureThreshold = 600;

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "123";
char pass[] = "1234567890";


void sendGPSLocation() {
  if (gps.location.isValid()) {
    float latitude = gps.location.lat();
    float longitude = gps.location.lng();
    String location = "Lat: " + String(latitude, 6) + ", Lon: " + String(longitude, 6);
    Blynk.logEvent("theft_loc","Motor theft detected! Location: " + location);
  } else {
    Blynk.logEvent("theft_motor", "Motor theft detected! Location latitude: 16.2469167, Longitude: 80.4069884");
  }
}


void checkSoilMoisture() {
  int soilMoistureValue = analogRead(soilMoisturePin);
  Serial.println("Soil Moisture Value: " + String(soilMoistureValue));
   Blynk.virtualWrite(V1,soilMoistureValue);
  if (soilMoistureValue < moistureThreshold) {
    digitalWrite(relayPin, LOW); 
  } else {
    digitalWrite(relayPin, HIGH);
  }
}

void checkIRSensor() {
  int irSensorValue = digitalRead(irSensorPin);
  if (irSensorValue == HIGH) { 
    Blynk.virtualWrite(V0,irSensorValue);
    Blynk.logEvent("theft","Motor movement detected! Possible theft.");
    sendGPSLocation();
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(soilMoisturePin, INPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(irSensorPin, INPUT);
  
  Blynk.begin(auth, ssid, pass);
  digitalWrite(relayPin, HIGH);

  timer.setInterval(10000L, checkSoilMoisture); 
  timer.setInterval(1000L, checkIRSensor);
}

void loop() {
  Blynk.run();
  timer.run();
  while (Serial.available() > 0) {
    gps.encode(Serial.read());
  }
}
