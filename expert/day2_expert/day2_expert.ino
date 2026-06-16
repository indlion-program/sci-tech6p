// ============================================================
// DAY 2 — EXPERT TEMPLATE  ▲ Ages 13-16
// "The Art of Scrambling"
// Implement a Caesar cipher in C++ — from scratch.
// ============================================================
#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "______";    // your SSID from Day 1
const char* password = "letmein123";

WebServer server(80);

// TASK: Implement caesarEncrypt(text, shift)
// Rules:
//   - Shift uppercase letters (A-Z) by 'shift' positions, wrapping around Z→A
//   - Shift lowercase letters (a-z) by 'shift' positions, wrapping around z→a
//   - Leave spaces, numbers, and punctuation unchanged
//   - Shift can be any number (use modulo 26 to handle large shifts)
// Example: caesarEncrypt("Hello", 3) → "Khoor"
String caesarEncrypt(String text, int shift) {
  // Your implementation here:

  return "";  // replace this with the actual result
}

void handleRoot() {
  String message = "My secret mission briefing";   // change this
  int shift = 7;
  String encrypted = caesarEncrypt(message, shift);

  String html = "<!DOCTYPE html><html><body style='font-family:monospace;padding:30px'>";
  html += "<h1>Caesar Cipher Server</h1>";
  html += "<p style='font-size:1.3em;background:#f5f5f5;padding:10px'>" + encrypted + "</p>";
  html += "<p><em>Shift: ??? Good luck figuring it out!</em></p>";
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

// VERBAL CHECK — explain these to a staff member:
// 1. Why is Caesar cipher easy to crack even without the key?
// 2. What is the difference between encoding (Base64) and encryption (Caesar)?
// 3. How would you modify this to do a Vigenère cipher instead?
// CHALLENGE: Add a second route /decode?text=xxx&shift=n that decrypts on-server
