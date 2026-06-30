// ============================================================
// DAY 4 — EXPLORER  * Ages 10-12
// "The Cyber Escape Room"
//
// Build a 2-page puzzle:
//   PAGE 1  (/)        a clue + a hidden Base64 path to the secret page
//   PAGE 2  (/secret)  the "YOU WIN!" page
//
// 1. Pick a secret path, e.g. /treasure
// 2. Encode it in tools/base64_encoder.html, paste it in the hidden comment.
// 3. If you change /secret below, change it in setup() too.
// ============================================================
#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "______";       // your name
const char* password = "letmein123";

WebServer server(80);

// Everything between  R"PAGE(  and  )PAGE"  is plain HTML.

// PAGE 1 - the clue page (the hidden Base64 path lives in the comment)
const char MAIN_PAGE[] = R"PAGE(

<!-- CLUE: ______ -->

<h1>______</h1>
<p>______</p>

)PAGE";

// PAGE 2 - the win page
const char WIN_PAGE[] = R"PAGE(

<h1 style="font-size:3em">YOU WIN!</h1>
<p>You cracked the puzzle!</p>
<p>Show this to a staff member!</p>

)PAGE";

void handleRoot() {
  server.send(200, "text/html", MAIN_PAGE);
}

void handleSecret() {
  server.send(200, "text/html", WIN_PAGE);
}

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  Serial.println(WiFi.softAPIP());
  server.on("/", handleRoot);
  server.on("/secret", handleSecret);   // change /secret if you used a different path
  server.begin();
}

void loop() {
  server.handleClient();
}
