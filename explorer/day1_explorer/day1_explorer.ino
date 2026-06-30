// ============================================================
// DAY 1 — EXPLORER  * Ages 10-12
// "The Invisible Network"
//
// 1. Put your name in the ssid below.
// 2. Type your own message in the PAGE below.
// 3. Upload, connect to your Wi-Fi, and visit 192.168.4.1
// ============================================================
#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "______";       // <-- your name, e.g. "Sarah_Network"
const char* password = "letmein123";   // everyone uses this password

WebServer server(80);

// This is your web page.
// Everything between  R"PAGE(  and  )PAGE"  is plain HTML.
// You can paste HTML straight from W3Schools here - no quotes to fix!
const char PAGE[] = R"PAGE(

<h1>______</h1>
<p>______</p>

)PAGE";

void handleRoot() {
  server.send(200, "text/html", PAGE);   // send your page to anyone who visits
}

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);           // start your Wi-Fi network
  Serial.println(WiFi.softAPIP());        // prints 192.168.4.1
  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
}
