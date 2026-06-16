// ============================================================
// DAY 3 — EXPERT TEMPLATE  ▲ Ages 13-16
// "Peeking Behind the Curtain"
// Multi-layer steganography: combine hidden HTML comment
// with a Caesar-encrypted flag AND a Base64-encoded hint.
// ============================================================
#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "______";    // your SSID
const char* password = "letmein123";

WebServer server(80);

// TASK: Reuse your caesarEncrypt from Day 2 (paste it here)
String caesarEncrypt(String text, int shift) {
  return "";  // paste your Day 2 implementation
}

void handleRoot() {
  // Your flag — follow the format WORD-WORD-NUMBER
  String plainFlag = "YOUR-FLAG-HERE";
  int shift = 5;  // choose your own shift

  // TASK: Caesar-encrypt the flag before hiding it
  String encryptedFlag = caesarEncrypt(plainFlag, shift);

  String html = "<!DOCTYPE html><html><body style='font-family:sans-serif;padding:30px'>";

  // TASK: Put the ENCRYPTED flag in a hidden HTML comment.
  // Solvers must: (1) find it in source, (2) know to decrypt it, (3) find the shift.
  html += "<!-- ENCODED: " + encryptedFlag + " -->";

  // TASK: Write a page that looks completely innocent
  // Add a subtle second hint if you want — but keep it ambiguous
  html += "<h1>______</h1>";
  html += "<p>______</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
}

// VERBAL CHECK:
// 1. How does adding Caesar on top of the HTML comment change the difficulty?
// 2. What additional information does the solver need to find the flag now?
// 3. Real CTFs often layer multiple encoding steps — can you add Base64 on top of Caesar?
