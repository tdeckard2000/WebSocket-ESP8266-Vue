/*
 * Connects to server using WebSocket Connection
 * Supports two-way communication from Vue App to ESP8266 and vice versa.
 *
 * 1. Update Wifi name and password
 * 2. Update secure web socket url or ip address
 * 3. Place a red led on pin D3 (off: Connected to wifi)
 * 4. Place a blue led on pin D5 (off: Connected to server)
 * 5. Ground D2 to send message to server
 * 6. Optional: To track lighting, a photo sensor can be connected to D6 and 3.3v, with
      a pulldown resistor from D6 to ground.
 * 7. Optional: To sound a buzzer, connect a buzzer to D7 and ground.
 *
 * ESP will constantly try to establish socket and wifi connections.
*/

#include <ESP8266WiFi.h>
#include <ArduinoWebsockets.h>
#define D2 4   // Button
#define D3 0   // Wifi Status LED
#define D4 2   // Built-in LED
#define D5 14  // LED
#define D6 12  // Photo Sensor
#define D7 13  // Buzzer

using namespace websockets;

const char* ssid = "*****";     //Wifi name
const char* password = "*****"; //Wifi pass
const char* wssUrl = "wss://websocket-5d15bc66efcd.herokuapp.com/";
int previousPinState = 0;
int previousLightSensorState = 0;
const long debounceInterval = 100;
unsigned long buttonDebounce = 0;
unsigned long lightDebounce = 0;

WebsocketsClient client;

void toggleInternalLED(int state) {
  digitalWrite(D4, state == 1 ? LOW : HIGH);
}

int getPinState(int pin) {
  int state = digitalRead(pin);
  if (state == LOW) {
    return 0;
  } else {
    return 1;
  }
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

void soundBuzzer() {
  for (int i = 0; i < 3 i++) {
    toggleBuzzer(1);
    delay(300);
    toggleBuzzer(0);
    delay(300);
  }
}

void handleMessage(String message) {
  Serial.println(message);
  if (message == "LED_ON") {
    toggleLED(1);
    sendMessage("ESP1 Received " + message);
  } else if (message == "LED_OFF") {
    toggleLED(0);
    sendMessage("ESP1 Received " + message);
  } else if (message == "REQUEST_ROOM_LIGHT_STATE") {
    int lightSensorState = getPinState(D6);
    sendLightSensorState(lightSensorState);
  } else if (message == "SOUND_BUZZER") {
    sendMessage("ESP1 Sounding Buzzer... ");
    soundBuzzer();
    sendMessage("ESP1 Done Sounding Buzzer");
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

int debounceTimePassed(unsigned long previousMillis) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= debounceInterval) {
    return 1;
  } else {
    return 0;
  }
}

void checkForUpdatedPinState() {
  int pinState = getPinState(D2);
  int lightSensorState = getPinState(D6);
  if (pinState != previousPinState && debounceTimePassed(buttonDebounce)) {
    buttonDebounce = millis();
    previousPinState = pinState;
    Serial.println(pinState);
    sendMessage(pinState == 0 ? "buttonDown" : "buttonUp");
  }
  if (lightSensorState != previousLightSensorState && debounceTimePassed(lightDebounce)) {
    lightDebounce = millis();
    previousLightSensorState = lightSensorState;
    sendLightSensorState(lightSensorState);
  }
}

void sendLightSensorState(int pinState) {
  sendMessage(pinState ? "roomLightsOn" : "roomLightsOff");
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

void toggleBuzzer(int on) {
  if (on) {
    digitalWrite(D7, HIGH);
  } else {
    digitalWrite(D7, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(D2, INPUT_PULLUP);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, INPUT);
  pinMode(D7, OUTPUT);
  toggleBuzzer(0);
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