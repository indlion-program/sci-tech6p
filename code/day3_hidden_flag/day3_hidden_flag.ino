// ==========================================================
// DAY 3 — COMPLETE REFERENCE CODE
// "Peeking Behind the Curtain" — Hide a flag in HTML source
// The flag is INVISIBLE on screen but shows in View Source!
// Right-click the page → View Page Source to find it.
// ==========================================================
#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "Camp_Network_1";
const char* password = "letmein123";

WebServer server(80);

void handleRoot() {
  String html = "<!DOCTYPE html><html><body style='font-family:sans-serif;padding:30px'>";

  // This comment is INVISIBLE on the page but visible in source!
  // Change YOUR-FLAG-HERE to something like: PURPLE-FALCON-99
  html += "<!-- FLAG: YOUR-FLAG-HERE -->";

  html += "<h1>Welcome to my server!</h1>";
  html += "<p>Everything is completely normal here.</p>";
  html += "<p>Nothing to see. Move along.</p>";
  html += "<p style='color:#aaa;font-size:0.8em'>Definitely no hidden messages anywhere.</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  Serial.println(WiFi.softAPIP());
  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
}
