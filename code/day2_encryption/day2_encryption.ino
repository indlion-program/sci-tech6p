// ==========================================================
// DAY 2 — COMPLETE REFERENCE CODE
// "The Art of Scrambling" — Serve a Base64-encoded message
// Use the tools/base64_encoder.html file to encode your message
// ==========================================================
#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "Camp_Network_1";
const char* password = "letmein123";

WebServer server(80);

void handleRoot() {
  // Paste YOUR Base64-encoded message here:
  String encoded = "Q2FtcCBDeWJlciBBY2FkZW15";   // = "Camp Cyber Academy"

  String html = "<!DOCTYPE html><html><body style='font-family:sans-serif;padding:30px'>";
  html += "<h1>Encrypted Message</h1>";
  html += "<p style='font-family:monospace;font-size:1.4em;background:#f0f0f0;padding:12px;border-radius:6px'>";
  html += encoded;
  html += "</p>";
  html += "<p><em>Can you decode this? You'll need the right tool...</em></p>";
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
