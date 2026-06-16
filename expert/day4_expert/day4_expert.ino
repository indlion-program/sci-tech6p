// ============================================================
// DAY 4 — EXPERT TEMPLATE  ▲ Ages 13-16
// "The Ultimate Cyber Escape Room"
// Expert requirement: 3 URL hops, not 2.
// LED on pin 2 MUST light up on the final win page.
//
// Puzzle chain you must build:
//   192.168.4.1          → visible clue + hidden encoded URL
//   192.168.4.1/level2   → second clue + second hidden encoded URL
//   192.168.4.1/win      → LED fires + YOU WIN message
// ============================================================
#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "______";
const char* password = "letmein123";

#define LED_PIN 2

WebServer server(80);

// TASK: Implement caesarEncrypt (paste from Day 2)
String caesarEncrypt(String text, int shift) {
  return "";  // paste your implementation
}

// LAYER 1 — Entry page: visible clue + hidden Base64-encoded path to /level2
void handleRoot() {
  // TASK: Build this page. Requirements:
  //   - HTML comment with Base64-encoded path "/level2"
  //     (encode it with tools/base64_encoder.html → paste result)
  //   - A creative story/clue that fits your theme (spy, dragon, space, etc.)
  //   - Must NOT give away what to look for too easily
}

// LAYER 2 — Middle page: second clue + Caesar-encrypted path to /win
void handleLevel2() {
  // TASK: Build this page. Requirements:
  //   - HTML comment with Caesar-encrypted path "/win" (choose your own shift)
  //   - Hint somewhere on the page about what kind of cipher was used
  //   - The shift number should NOT be written anywhere — make them guess!
}

// LAYER 3 — Final page: LED triggers + YOU WIN
void handleWin() {
  // TASK: Fire the LED and serve a win page.
  // LED wiring: GPIO pin 2 → 330Ω resistor → LED anode → LED cathode → GND
  digitalWrite(LED_PIN, HIGH);

  // TASK: Build the win page — make it dramatic!
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  WiFi.softAP(ssid, password);
  Serial.println(WiFi.softAPIP());

  // TASK: Register all three routes
  server.on("/", handleRoot);
  server.on("/level2", handleLevel2);
  server.on("/win", handleWin);

  server.begin();
}

void loop() {
  server.handleClient();
}

// VERBAL CHECK:
// 1. Walk a staff member through the full solution path of your puzzle.
// 2. What encoding is on each hop? Why did you choose it?
// 3. If you had more time, what 4th layer would you add?
// 4. How would a real pentester approach your puzzle if they had 5 minutes?
