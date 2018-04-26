#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

bool lost = false;

const int led = D4;
const int buzzer = D5;

/* Set these to your desired credentials. */
const char *ssid = "ESPap"; // You can change it according to your ease
const char *password = "12345678"; // You can change it according to your ease

ESP8266WebServer server(80); // establishing server at port 80 (HTTP protocol's default port)

// This function will be called whenever send a GET reuest to 192.168.4.1/status within the local area connection of this ESP module.
void statusGet()
{
  if(lost)
    server.send(200, "text/json", "{\"lost:\" true}");
  else
    server.send(200, "text/json", "{\"lost:\" false}");
}

void statusPut() {
  String requestBody = server.arg("plain");

  // Sloppy parsing, but works for now.
  if(requestBody.indexOf("true") > 0) {
    lost = true;
  } else {
    lost = false;
    digitalWrite(led, LOW);
  }

  Serial.println(requestBody);

  server.send(204);
}

void beep() {
    for(int i=0;i<80;i++)
    {
      digitalWrite(buzzer,HIGH);
      delay(1);//wait for 1ms
      digitalWrite(buzzer,LOW);
      delay(1);//wait for 1ms
    }

    for(int i=0;i<100;i++)
    {
      digitalWrite(buzzer,HIGH);
      delay(2);//wait for 2ms
      digitalWrite(buzzer,LOW);
      delay(2);//wait for 2ms
    }
}

void setup() {
  delay(1000);
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(115200);
  Serial.println();

  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password); // --> This line will create a WiFi hotspot.

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  
  server.on("/status", HTTP_GET, statusGet);
  server.on("/status", HTTP_PUT, statusPut);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  if(lost) {
    beep();
    digitalWrite(led, !digitalRead(led));
  }
  
  server.handleClient();
}
