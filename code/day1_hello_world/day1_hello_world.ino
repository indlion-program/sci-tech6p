// ==========================================================
// DAY 1 — COMPLETE REFERENCE CODE
// "The Invisible Network" — Access Point Web Server
// Flash this and connect any device to see your web page!
// ==========================================================
#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "Camp_Network_1";   // <-- change to your name
const char* password = "letmein123";

WebServer server(80);

void handleRoot() {
  String html = "<!DOCTYPE html><html><body style='font-family:sans-serif;padding:30px'>";
  html += "<h1>Hello World!</h1>";
  html += "<p>You connected to my secret network!</p>";
  html += "<p>This page is being served from a tiny ESP32 chip.</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());   // always 192.168.4.1
  server.on("/", handleRoot);
  server.begin();
  Serial.println("Server started!");
}

void loop() {
  server.handleClient();
}
