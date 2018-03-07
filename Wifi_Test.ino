#include <ESP8266WiFi.h>
 
const char* ssid     = "What Team?";
const char* password = "12Sirius3";
 
const char* host = "wifitest.adafruit.com";

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(115200);
  delay(100);
 
  // We start by connecting to a WiFi network
 
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
}

int value = 0;

void loop() {
  stopLocalAP();
  // put your main code here, to run repeatedly:
    digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
                                    // but actually the LED is on; this is because 
                                    // it is acive low on the ESP-01)
    delay(1000);                      // Wait for a second
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Still Connected");
    digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
    delay(2000);
  }
  else {
    Serial.println("Disconnected");
  }
}
