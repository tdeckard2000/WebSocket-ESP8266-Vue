/*
 * Connects to server using WebSocket Connection
 * Supports two-way communication from Vue App to ESP8266 and vice versa.
 *
 * 1. Update certificate in the code to match the server's certificate
 * Can get certificate from https://www.ssllabs.com/ssltest/
 *
 * 2. Update Wifi name and password
 *
 * Ground D2 to see updates
 * LED ON: Unable to connect to Wifi OR Server
 * LED OFF: Connected to Server
 * Will constantly try to establish a connection.
*/

#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <WiFiClientSecure.h>
#define D2 4  //LED
#define D4 2

const char* ssid = "******";  //Wifi name
const char* password = "******"; //Wifi pass
const char* serverUrl = "websocket-5d15bc66efcd.herokuapp.com";
const int serverPort = 443; //heroku uses 443 or 80 or 29098
bool webSocketConnected = false;
int previousPinState = 0;
const long interval = 100;
unsigned long previousMillis = 0;

// WiFiClientSecure client;
WebSocketsClient webSocket;

void toggleLED(int state) {
  digitalWrite(D4, state == 1 ? LOW : HIGH);
}

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  Serial.println("type");
  Serial.println(type);
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("Disconnected from WebSocket server");
      toggleLED(1);
      webSocketConnected = false;
      // connectToWifi();
      // connectToServer();
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

const char ENDPOINT_CA_CERT[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFyzCCBLOgAwIBAgIQBNgHf0MpKXvMJ7Ag6jcHnzANBgkqhkiG9w0BAQsFADA8MQswCQYDVQQG
EwJVUzEPMA0GA1UEChMGQW1hem9uMRwwGgYDVQQDExNBbWF6b24gUlNBIDIwNDggTTAzMB4XDTI0
MDMwMjAwMDAwMFoXDTI1MDMzMDIzNTk1OVowGjEYMBYGA1UEAwwPKi5oZXJva3VhcHAuY29tMIIB
IjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAtQ0K9an+bZxwGjRaqG6MdFj/POh0Ml8KAoDo
ZzeV9FAJe79zFfixTOrChcbtqbz5ZDlaxPBpEi2P2jjeeaQvblK3BrnXQomwaIDDUTEq+QJ5TDZ+
EYrHQidXE3ONH0zfvhk+3NWLNtTkYGgptlmmSDobFIVUIFzEEDTOmEA1qbLEXL1Gwv4JuZCL9Nte
k4TZ5yzC/gQ/lx8rn+bI54frAexitdmMQPkHP9PJEmv1yvRhhKH+lOsHsAN9nXpPE4kfIHU7ySJp
MPN8wQsompS2Mw6WE+5obThtpHIBMhrlKAOyyqEgVaJN1D5Q7ehVh0RrK0e7YFl31bpd8Hw8h/Af
9QIDAQABo4IC6TCCAuUwHwYDVR0jBBgwFoAUVdkYX9IczAHhWLS+q9lVQgHXLgIwHQYDVR0OBBYE
FBj+YN7NeOIzlqwYiEkQbJjad8u5MBoGA1UdEQQTMBGCDyouaGVyb2t1YXBwLmNvbTATBgNVHSAE
DDAKMAgGBmeBDAECATAOBgNVHQ8BAf8EBAMCBaAwHQYDVR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUF
BwMCMDsGA1UdHwQ0MDIwMKAuoCyGKmh0dHA6Ly9jcmwucjJtMDMuYW1hem9udHJ1c3QuY29tL3Iy
bTAzLmNybDB1BggrBgEFBQcBAQRpMGcwLQYIKwYBBQUHMAGGIWh0dHA6Ly9vY3NwLnIybTAzLmFt
YXpvbnRydXN0LmNvbTA2BggrBgEFBQcwAoYqaHR0cDovL2NydC5yMm0wMy5hbWF6b250cnVzdC5j
b20vcjJtMDMuY2VyMAwGA1UdEwEB/wQCMAAwggF/BgorBgEEAdZ5AgQCBIIBbwSCAWsBaQB3AE51
oydcmhDDOFts1N8/Uusd8OCOG41pwLH6ZLFimjnfAAABjf4B8jEAAAQDAEgwRgIhAIuFi58g0bTy
7IH2DPscFa0qMe+wEnF4uACxo4IIqf3vAiEAwfHp0TPCtQS6oLH4FtHP3RmnmCUImDyjNt8K6avj
BhUAdwB9WR4S4XgqexxhZ3xe/fjQh1wUoE6VnrkDL9kOjC55uAAAAY3+AfJJAAAEAwBIMEYCIQDb
PonR69l7mCu+vSeXWy0h23QaX1ajfabCfmY8Zr6uxgIhALa5rfVVMHXMtX5wyvGaaR5+oAITyuIh
3vvwXyo4h5xQAHUA5tIxY0B3jMEQQQbXcbnOwdJA9paEhvu6hzId/R43jlAAAAGN/gHycwAABAMA
RjBEAiBM16ZkO8Dsm5APab5h68GTBl6i+ysS0gYQyS1vC2olaAIgTzpfhetPsYrZgTRHStEcaH1h
zKUkmO3d5MJBTYJUrdEwDQYJKoZIhvcNAQELBQADggEBAGPBy9gzssF7rNc1Se6nkhyyahNiNNF9
rTSiCcJ5Vc6HB+yHrhBVAv+Rv9RtMzPHZmDqPcT+QHKGsFqkVm9APUYAIzB9bvKGI+AN28Bo7Twn
9gO+PaBJE+N4RFuIrqcM3lvGmDSy6/FmGmX3Pz9ivtxNZJgxugjSQhCRL4/08Wbqe2nsbHxhF3og
fGYfohUzQWCSE5Be3fkbgdkTuUfga56UTvsB+S5ShoAVCyMYa585ba/qedzdvIFXK95H3RcBlH9+
exge5bo0dgzlF1P4+jMwbwaHNA/kqM+0GfLjtg5cJG3bREzzdlq/hvUTN6k13YAA6Nw/1jo97YGc
/r69/1g=
-----END CERTIFICATE-----
)EOF";

void connectToServer() {
  webSocket.beginSslWithCA(serverUrl, serverPort, "/", ENDPOINT_CA_CERT);
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