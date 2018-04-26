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


// Writing a simple HTML page.
char HTML[] = "<html><body><h1>WiFinder</h1><button><a href=\"status\">Toggle LED</a></button></body></html>"; // --> Simple HTML page


// This function will be called whenever anyone requests 192.168.4.1 within the local area connection of this ESP module.
void handleRoot() 
{
  server.send(200, "text/html",HTML);
}

// This function will be called whenever anyone requests 192.168.4.1/toggle within the local area connection of this ESP module.
void status()
{
  lost = !lost;
  digitalWrite(led,!digitalRead(led));

  if(lost)
    server.send(200, "text/json", "{\"lost:\" true}");
  else
    server.send(200, "text/json", "{\"lost:\" false}");
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
  pinMode(led,OUTPUT);
  pinMode(buzzer,OUTPUT);
  Serial.begin(115200);
  Serial.println();

  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password); // --> This line will create a WiFi hotspot.

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  
  server.on("/", handleRoot);
  server.on("/status", status);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  if(lost)
    beep();
  
  server.handleClient();
}
