#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

bool lost = false;

const int led = D4;
const int buzzer = D5;

// AP configuration
const int channel = 6;
const char *ssid = "WiFinder";
const char *password = "12345678";

// Webserver listens on port 80
ESP8266WebServer server(80);

// GET /status
void statusGet()
{
  if(lost)
    server.send(200, "text/json", "{\"lost\": true}");
  else
    server.send(200, "text/json", "{\"lost\": false}");
}

void statusPut() {
  String requestBody = server.arg("plain");

  // Sloppy JSON parsing, but works for now.
  if(requestBody.indexOf("true") > 0 && requestBody.indexOf("false") < 0) {
    lost = true;
  } else if(requestBody.indexOf("false") > 0 && requestBody.indexOf("true") < 0) {
    lost = false;
    digitalWrite(led, LOW);
  } else {
    server.send(400);
    return;
  }

  server.send(204);
}

void beep() {
    for(int i = 0; i < 80; i++) {
      digitalWrite(buzzer, HIGH);
      delay(1);
      digitalWrite(buzzer, LOW);
      delay(1);
    }

    for(int i = 0; i < 100; i++) {
      digitalWrite(buzzer, HIGH);
      delay(2);
      digitalWrite(buzzer, LOW);
      delay(2);
    }
}

void setup() {
  delay(1000);

  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);

  Serial.begin(115200);
  Serial.println();

  Serial.print("Configuring access point... ");
  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password, channel);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  server.on("/status", HTTP_GET, statusGet);
  server.on("/status", HTTP_PUT, statusPut);
  server.begin();
  Serial.println("HTTP server started!");
}

void loop() {
  if(lost) {
    beep();
    digitalWrite(led, !digitalRead(led));
  }

  server.handleClient();
}
