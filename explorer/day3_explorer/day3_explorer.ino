// ============================================================
// DAY 3 — EXPLORER  * Ages 10-12
// "Peeking Behind the Curtain"
//
// Hide a secret flag: INVISIBLE on the page, but it shows up
// when someone clicks "View Page Source".
//
// 1. Put your flag in the hidden comment below (WORD-WORD-NUMBER).
// 2. Write an innocent-looking message for the rest of the page.
// 3. Upload, visit 192.168.4.1, then try "View Page Source".
// ============================================================
#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "______";       // your name from Day 1
const char* password = "letmein123";

WebServer server(80);

// Everything between  R"PAGE(  and  )PAGE"  is plain HTML.
// The <!-- ... --> line is a hidden comment - invisible on screen!
const char PAGE[] = R"PAGE(

<!-- FLAG: YOUR-FLAG-HERE -->

<h1>______</h1>
<p>______</p>

)PAGE";

void handleRoot() {
  server.send(200, "text/html", PAGE);
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
