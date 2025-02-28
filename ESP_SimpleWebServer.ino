/*************************************************************************************************************************
*********************************    Author  : Ehab Magdy Abdullah                      **********************************
*********************************    Linkedin: https://www.linkedin.com/in/ehabmagdyy/  **********************************
*********************************    Youtube : https://www.youtube.com/@EhabMagdyy      **********************************
**************************************************************************************************************************/

#if defined(ESP32)
#include <WiFi.h>
#else if defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

// WiFi Credentials
#define SSID                "SSID"
#define PASSWORD            "PASSWORD"

#define LED_BUILTIN         2

WiFiServer server(80);      // Create a web server on port 80

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin(); // Start the web server
}

void loop()
{
  WiFiClient client = server.available(); // Check for client connections

  if (client) {
    // Read the first line of the HTTP request
    String request = client.readStringUntil('\r');
    client.flush();

    // Handle the request to control the LED
    if (request.indexOf("GET /ledon") >= 0) {
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.println("Received: Led ON");
    } else if (request.indexOf("GET /ledoff") >= 0) {
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println("Received: Led OFF");
    }

    // Determine LED status
    String ledStatus = (digitalRead(LED_BUILTIN) == HIGH) ? "ON" : "OFF";

    // HTML response
    String html = "";
    html += "<!DOCTYPE html>";
    html += "<html><head>";
    html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
    html += "<style>";
    html += "body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; background-color: white; }";
    html += ".button { display: inline-block; padding: 10px 20px; font-size: 20px; margin: 10px; color: white; border: none; cursor: pointer; }";
    html += ".on { background-color: green; } .off { background-color: red; }";
    html += "</style></head><body>";
    html += "<h1>ESP LED Control</h1>";
    html += "<p>LED Status: <strong>" + ledStatus + "</strong></p>";
    html += "<a href=\"/ledon\"><button class=\"button on\">LED ON</button></a>";
    html += "<a href=\"/ledoff\"><button class=\"button off\">LED OFF</button></a>";
    html += "</body></html>";

    // Send the HTTP response header
    client.printf("HTTP/1.1 200 OK\r\n"
                  "Content-Type: text/html\r\n"
                  "Content-Length: %u\r\n"
                  "Connection: close\r\n\r\n", html.length());
    
    // Send the HTML content
    client.print(html);
    
    delay(1);
    client.stop();
  }


  // Auto-reconnect if Wi-Fi disconnects
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi Disconnected! Reconnecting...");
    WiFi.disconnect();
    WiFi.reconnect();
  }
}
