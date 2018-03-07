#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include "../generic/common.h"

#define PIN_WIRE_SDA (4)
#define PIN_WIRE_SCL (5)



static const uint8_t D0   = 16;
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D3   = 0;
static const uint8_t D4   = 2;
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13;
static const uint8_t D8   = 15;
static const uint8_t D9   = 3;
static const uint8_t D10  = 1;

#endif /* Pins_Arduino_h */

#include <ESP8266WiFi.h>
extern "C" {
#include "user_interface.h"
}

os_timer_t myTimer;

int lightValue[] = {0,0};

bool tickOccured;
 
const char* ssid     = "What Team?";
const char* password = "12Sirius3";

const char* host = "10.0.0.6";
const int httpPort = 80;

void wifiCheck(){
  digitalWrite(LED_BUILTIN, HIGH);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(D2, HIGH);
    delay(250);
    digitalWrite(D2, LOW);
    delay(250);
    Serial.print(".");
  }
}

void wifiConnect(){
  delay(100);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  wifiCheck();
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(D2,OUTPUT);
  pinMode(D7,INPUT_PULLUP);
  pinMode(D1,INPUT_PULLUP);
  pinMode(D3,INPUT_PULLUP);

  Serial.begin(115200);
  tickOccured = false;
  user_init();
  wifiConnect();
  

}

void timerCallback(void *pArg) {

      tickOccured = true;

} // End of timerCallback

void user_init(void) {

      os_timer_setfn(&myTimer, timerCallback, NULL);


      os_timer_arm(&myTimer, 20000, true);
} 

void requestStatus(int light, WiFiClient client){
  // We now create a URI for the request
      digitalWrite(D2, HIGH);
      String url = "/api/QYCMf3dLRS0WrN7mORGm7msvEX4HampeoRFiv6Ih/lights/"+String(light);
      Serial.print("Requesting URL: ");
      Serial.println(url);
  
      // This will send the request to the server
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" + 
                   "Connection: close \r\n\r\n");
            
      digitalWrite(D2, LOW);
}

////// Value Checkers

String getJSONResponse(int light, WiFiClient client){
  requestStatus(light, client);
  while(client.available()){
    String line = client.readStringUntil('\r');
    if(line.indexOf("state")>=0){
      Serial.print("arrived");
      delay(50);
      return line;
    }
  }
}

void parseJSONResponse(String line, WiFiClient client){
  int active;
  if(line.indexOf("\"on\":true")>=0){
  
    active = 1;
    
    }else if(line.indexOf("\"on\":false")>=0){
    
    active = 0;
    
    }
  
  int currentChar = line.indexOf("\"bri\":")+8;
  
  int value = int(line[currentChar]);
  int bri[3] = {-1,-1,-1};
  int counter = 0;
  while (value>47 && value<58){
    bri[counter] = value-48;
    currentChar +=1;
    counter +=1;
    value = int(line[currentChar]);
  }
  int brightness;
  if (bri[2] == -1){
    if (bri[1] == -1){
      brightness = bri[0];
    }
    else{
      brightness = bri[0]*10 + bri[1];
    }
  }else{
    brightness = bri[0]*100+bri[1]*10+bri[2];
  }
  
  lightValue[0] = active;
  lightValue[1] = brightness;
}

void checkValues(int light, WiFiClient client){
  parseJSONResponse(getJSONResponse(light, client),client);
}

////// Value Checkers


////// Sender Function

void sendRequest(int light, String content, WiFiClient client){
  String url = "/api/QYCMf3dLRS0WrN7mORGm7msvEX4HampeoRFiv6Ih/lights/"+String(light)+"/state";
  if(client.connect(host, httpPort)){
    client.print(String("PUT ") + url + " HTTP/1.1\r\n" +
                "Connection: close\r\n" +
                "Host: " + host + "\r\n" + 
                "Content-Length: "+content.length()+"\r\n "+
                "Content-Type: text/plain;charset=UTF-8\r\n\r\n"+
                content);
  }
}

// Sender Function END

//////# Toggle Light Function
void lightToggle(int light, WiFiClient client){
  
  if(lightValue[0] == 1){
    String content = "{\"on\":false}";

    sendRequest(light, content, client);
    lightValue[0]=0;
    
  }
  else{
    String content = "{\"on\":true}";
    sendRequest(light, content, client);
    lightValue[0]=1;
    

  }
  
  
}

//////// Toggle Light Function END

////# Brightness Up Function

void brightnessUp(int light, WiFiClient client){

  int bri = lightValue[1]+5;
  bri = 254<bri ? 254:bri; 
  String content = "{\"bri\":"+String(bri)+"}";

  sendRequest(light, content, client);
  int check2 = digitalRead(D1);
  while (check2==LOW){
    bri = bri+8;
    bri = 254<bri ? 254:bri; 
    String content = "{\"bri\":"+String(bri)+"}";

    sendRequest(light, content, client);
    delay(100);
    check2 = digitalRead(D1);
  }
  lightValue[1] = bri;
}


////# Brightness Down Function
void brightnessDown(int light, WiFiClient client){

  int bri = lightValue[1]-5;
  bri = 0>bri ? 0:bri; 
  String content = "{\"bri\":"+String(bri)+"}";

  sendRequest(light, content, client);
  int check3 = digitalRead(D3);
  while (check3==LOW){
    bri = bri-8;
    bri = 0>bri ? 0:bri; 
    String content = "{\"bri\":"+String(bri)+"}";

    sendRequest(light, content, client);
    delay(100);
    check3 = digitalRead(D3);
  }
  lightValue[1] = bri;
  

  
}

////////////MAIN LOOP///////////

int check  = 0;
int check2 = 0;
int check3 = 0;
int currentLight = 0;
int value = 0;


void loop() {
  if (tickOccured == true){
  WiFiClient client;
    if (!client.connect(host, httpPort)) {
      Serial.println("connection failed");
      return;
    }
    checkValues(3, client );
    if (lightValue[0]==0){
      
    }
  tickOccured = false;
  }
  // put your main code here, to run repeatedly:
  check = digitalRead(D7);
  if (check == LOW){  
    // Use WiFiClient class to create TCP connections
      WiFiClient client;
      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
      }
      lightToggle(3, client);

    delay(250);
  }
  check2 = digitalRead(D1);
  if (check2 == LOW){
    // Use WiFiClient class to create TCP connections
      WiFiClient client;
      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
      }
      brightnessUp(3, client);

      delay(250);
    
  }
  check3 = digitalRead(D3);
  if (check3 == LOW){
    
    // Use WiFiClient class to create TCP connections
      WiFiClient client;
      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
      }
      brightnessDown(3, client);

  }  
  
  
  
}

