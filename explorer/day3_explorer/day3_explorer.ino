// ============================================================
// DAY 3 — EXPLORER TEMPLATE  ★ Ages 10-12
// "Peeking Behind the Curtain"
// Your flag must be INVISIBLE on the page but VISIBLE in source!
//
// Flag format:  WORD-WORD-NUMBER
// Example:      BLUE-ROCKET-42
// Make it your own!
// ============================================================
#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "______";       // your name from Day 1
const char* password = "letmein123";

WebServer server(80);

void handleRoot() {
  String html = "<!DOCTYPE html><html><body style='font-family:sans-serif;padding:30px'>";

  // STEP: Replace YOUR-FLAG-HERE with your flag (WORD-WORD-NUMBER)
  //       This line is a hidden comment — invisible on screen!
  html += "<!-- FLAG: YOUR-FLAG-HERE -->";

  // STEP: Change the message on the page to something innocent-looking
  html += "<h1>______</h1>";
  html += "<p>______</p>";
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
