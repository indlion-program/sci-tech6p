// ============================================================
// DAY 2 — EXPLORER  * Ages 10-12
// "The Art of Scrambling"
//
// 1. Open tools/base64_encoder.html and encode a secret message.
// 2. Paste the encoded text in place of ______ in the PAGE below.
// 3. Upload, visit 192.168.4.1 - can a friend decode it?
// ============================================================
#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "______";       // your name from Day 1
const char* password = "letmein123";

WebServer server(80);

// Everything between  R"PAGE(  and  )PAGE"  is plain HTML.
// Paste your Base64 code in place of ______ below.
const char PAGE[] = R"PAGE(

<h1>My Secret Message</h1>
<p style="font-family:monospace;font-size:1.3em">______</p>
<p>Can you figure out what this says?</p>

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
