/*
 * Connects to server using WebSocket Connection
 * Supports two-way communication from Vue App to ESP8266 and vice versa.
 *
 * 1. Update Wifi name and password
 * 2. Update secure web socket url or ip address
 * 3. Place a red led on pin D3 (off: Connected to wifi)
 * 4. Place a blue led on pin D5 (off: Connected to server)
 * 5. Ground D2 to send message to server
 *
 * Will constantly try to establish socket and wifi connections.
*/

#include <ESP8266WiFi.h>
#include <ArduinoWebsockets.h>
#define D2 4  // Button
#define D3 0  // Wifi Status LED
#define D4 2  // Built-in LED
#define D5 14 // LED

using namespace websockets;

const char* ssid = "***REMOVED***"; //Wifi name
const char* password = "***REMOVED***"; //Wifi pass
const char* wssUrl = "wss://websocket-5d15bc66efcd.herokuapp.com/";
int previousPinState = 0;
const long interval = 100;
unsigned long previousMillis = 0;

WebsocketsClient client;

void toggleInternalLED(int state) {
  digitalWrite(D4, state == 1 ? LOW : HIGH);
}

int getPinState(int pin) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    int state = digitalRead(pin);
    if (state == LOW) {
      return 0;
    } else if (state == HIGH) {
      return 1;
    }
  }
  return previousPinState;
}

void connectToWifi() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }
  Serial.println();
  Serial.println("Connecting to Wifi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    delay(500);
  }
  toggleWifiLED(0);
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void handleMessage(String message) {
  Serial.println(message);
  if (message == "LED_ON") {
    toggleLED(1);
  } else if (message == "LED_OFF") {
    toggleLED(0);
  }
}

void connectToServer() {
  Serial.println("Connecting to server... ");
  bool connected = client.connect(wssUrl);
  client.onMessage([](WebsocketsMessage msg) {
    Serial.println("Got Message " + msg.data());
    handleMessage(msg.data());
  });
}

void sendMessage(String message) {
  client.send(message);
}

void checkForUpdatedPinState() {
  int pinState = getPinState(D2);
  if (pinState != previousPinState) {
    previousPinState = pinState;
    Serial.println(pinState);
    sendMessage(pinState == 0 ? "Button Down" : "Button Up");
  }
}

void toggleWifiLED(int on) {
  if (on) {
    digitalWrite(D3, HIGH);
  } else {
    digitalWrite(D3, LOW);
  }
}

void toggleLED(int on) {
  if (on) {
    digitalWrite(D5, HIGH);
  } else {
    digitalWrite(D5, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(D2, INPUT_PULLUP);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  toggleWifiLED(1);
  connectToWifi();
  connectToServer();
  toggleLED(0);
}

void loop() {
  if (client.available()) {
    checkForUpdatedPinState();
    toggleInternalLED(0);
    client.poll();
  } else {
    toggleInternalLED(1);
    if (WiFi.status() != WL_CONNECTED) {
      toggleWifiLED(1);
      connectToWifi();
    };
    connectToServer();
  }
}