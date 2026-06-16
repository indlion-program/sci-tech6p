// ============================================================
// DAY 1 — EXPLORER TEMPLATE  ★ Ages 10-12
// "The Invisible Network"
// Fill in every blank marked with  ______
// ============================================================
#include <WiFi.h>
#include <WebServer.h>

// STEP 1: Change "YourName_Network" to include YOUR first name
//         so it shows up as YOUR Wi-Fi in the list!
const char* ssid     = "______";      // example: "Sarah_Network"
const char* password = "letmein123";  // everyone uses the same password

WebServer server(80);

// This function runs every time someone visits your web page
void handleRoot() {
  // STEP 2: Replace the message inside the quotes.
  //         Be creative! It will show on the page.
  server.send(200, "text/html", "<h1>______</h1><p>______</p>");
}

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);          // start your Wi-Fi network
  Serial.println(WiFi.softAPIP());       // prints 192.168.4.1
  server.on("/", handleRoot);            // connect the "/" page to your function
  server.begin();                        // start the server!
}

// This runs over and over to keep the server alive
void loop() {
  server.handleClient();
}
