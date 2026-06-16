// ============================================================
// DAY 4 — EXPLORER TEMPLATE  ★ Ages 10-12
// "The Ultimate Cyber Escape Room"
//
// Build a 2-layer puzzle:
//   LAYER 1 — Main page: a visible clue or riddle
//   LAYER 2 — Hidden comment: Base64-encoded path to the win page
//   LAYER 3 — /secret page: "YOU WIN!" message
//
// How to create your hidden clue:
//   1. Decide on your secret URL — example: /treasure
//   2. Open tools/base64_encoder.html
//   3. Type  /treasure  and click Encode
//   4. Paste the result into the hidden comment below
//   5. Register your secret URL in setup() too!
// ============================================================
#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "______";       // your name
const char* password = "letmein123";

WebServer server(80);

// LAYER 1 — The main puzzle page
void handleRoot() {
  String html = "<!DOCTYPE html><html><body style='font-family:sans-serif;padding:30px'>";

  // STEP: Paste your Base64-encoded secret path here
  html += "<!-- CLUE: ______ -->";

  // STEP: Write a mysterious clue or riddle here
  html += "<h1>______</h1>";
  html += "<p>______</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

// LAYER 3 — The win page (change /secret to match your secret URL)
void handleSecret() {
  String html = "<!DOCTYPE html><html><body style='font-family:sans-serif;padding:30px;text-align:center'>";
  html += "<h1 style='font-size:3em'>YOU WIN!</h1>";
  html += "<p>You cracked ______'s puzzle!</p>";    // put your name here
  html += "<p>Show this to a staff member!</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  Serial.println(WiFi.softAPIP());
  server.on("/", handleRoot);
  server.on("/secret", handleSecret);   // STEP: change /secret to match your URL
  server.begin();
}

void loop() {
  server.handleClient();
}
