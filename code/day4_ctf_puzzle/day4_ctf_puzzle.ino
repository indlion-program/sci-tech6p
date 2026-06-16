// ==========================================================
// DAY 4 — COMPLETE REFERENCE CODE
// "The Ultimate Cyber Escape Room" — 3-Layer CTF Puzzle
//
// LAYER 1: Main page  → visible clue
// LAYER 2: HTML comment → Base64-encoded URL (/secret)
// LAYER 3: /secret page → WIN message + LED lights up!
//
// Puzzle flow for the solver:
//   1. Visit 192.168.4.1
//   2. Read the clue on the page
//   3. View Page Source → find the Base64 comment
//   4. Decode the Base64 → get the secret URL path
//   5. Visit 192.168.4.1/secret → WIN!
// ==========================================================
#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "Camp_Network_1";   // <-- your name!
const char* password = "letmein123";

#define LED_PIN 2   // Built-in LED on most ESP32 boards

WebServer server(80);

// LAYER 1 — Public clue page
void handleRoot() {
  String html = "<!DOCTYPE html><html><body style='font-family:monospace;background:#111;color:#0f0;padding:30px'>";

  // LAYER 2 — encoded path hidden in source. "L3NlY3JldA==" decodes to "/secret"
  // Change this to encode a different path if you want!
  html += "<!-- CLUE: L3NlY3JldA== -->";

  html += "<h1>The Secret Server</h1>";
  html += "<p>Welcome, agent. Your mission: find the hidden page.</p>";
  html += "<p>The answer lies where the screen cannot show you.</p>";
  html += "<p><em>Hint: look at what the browser <strong>receives</strong>, not what it shows...</em></p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

// LAYER 3 — Secret win page (reached by decoding the Base64 clue)
void handleSecret() {
  digitalWrite(LED_PIN, HIGH);   // Light up the LED!

  String html = "<!DOCTYPE html><html><body style='font-family:monospace;background:#111;color:#0f0;padding:30px;text-align:center'>";
  html += "<h1 style='font-size:3em'>YOU WIN!</h1>";
  html += "<p style='font-size:1.4em'>You cracked the puzzle. Well played, agent.</p>";
  html += "<p>Show this screen to a staff member for your point!</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  WiFi.softAP(ssid, password);
  Serial.print("CTF server running at: ");
  Serial.println(WiFi.softAPIP());
  server.on("/", handleRoot);
  server.on("/secret", handleSecret);   // the hidden win page
  server.begin();
}

void loop() {
  server.handleClient();
}
