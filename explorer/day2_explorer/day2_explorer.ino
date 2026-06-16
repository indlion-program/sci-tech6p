// ============================================================
// DAY 2 — EXPLORER TEMPLATE  ★ Ages 10-12
// "The Art of Scrambling"
// Steps:
//   1. Write your secret message on paper
//   2. Open tools/base64_encoder.html in Chrome (no internet needed)
//   3. Paste your message and click Encode
//   4. Copy the result and paste it into the blank below
// ============================================================
#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "______";       // your name from Day 1
const char* password = "letmein123";

WebServer server(80);

void handleRoot() {
  // STEP: Paste your Base64-encoded message here (between the quotes)
  String encoded = "______";

  String html = "<!DOCTYPE html><html><body style='font-family:sans-serif;padding:30px'>";
  html += "<h1>My Encrypted Message</h1>";
  html += "<p style='font-family:monospace;font-size:1.3em;background:#f0f0f0;padding:12px;border-radius:6px'>";
  html += encoded;
  html += "</p>";
  html += "<p><em>Can you figure out what this says?</em></p>";
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
