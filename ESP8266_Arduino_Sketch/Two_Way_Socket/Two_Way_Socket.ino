/*
 * Connects to server using TCP
 * Ground D2 to see updates
 * LED ON: Unable to connect to Wifi OR Server
 * LED OFF: Connected to Server
 * Will constantly try to establish a connection.
*/

#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#define D2 4  //LED
#define D4 2

const char* ssid = "***";  //Wifi name
const char* password = "***"; //Wifi pass
const char* serverUrl = "10.20.115.23"; //ex. 10.20.122.11
const int serverPort = 3000;
bool webSocketConnected = false;
int previousPinState = 0;
const long interval = 100;
unsigned long previousMillis = 0;

WebSocketsClient webSocket;

void toggleLED(int state) {
  digitalWrite(D4, state == 1 ? LOW : HIGH);
}

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  Serial.println("type");
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("Disconnected from WebSocket server");
      toggleLED(1);
      webSocketConnected = false;
      break;
    case WStype_CONNECTED:
      Serial.println("Connected to WebSocket server");
      webSocket.sendTXT("Hello from ESP8266");
      toggleLED(0);
      webSocketConnected = true;
      break;
    case WStype_TEXT:
      Serial.printf("Message from server: %s\n", payload);
      break;
    case WStype_ERROR:
      Serial.println("Error occurred with WebSocket");
      break;
  }
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
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void connectToServer() {
  webSocket.begin(serverUrl, serverPort, "/");
  webSocket.onEvent(webSocketEvent);
}

void sendMessage(String message) {
  webSocket.sendTXT(message);
}

void checkForUpdatedPinState() {
  int pinState = getPinState(D2);
  if (pinState != previousPinState) {
    previousPinState = pinState;
    Serial.println(pinState);
    sendMessage(pinState == 0 ? "Button Down" : "Button Up");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(D2, INPUT_PULLUP);
  pinMode(D4, OUTPUT);
  connectToWifi();
  connectToServer();
}

void loop() {
  webSocket.loop();  // Maintains WebSocket connection
  checkForUpdatedPinState();
  if (!webSocketConnected && WiFi.status() != WL_CONNECTED) {
    connectToWifi();
  }
}