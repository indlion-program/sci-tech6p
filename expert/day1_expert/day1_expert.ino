// ============================================================
// DAY 1 — EXPERT TEMPLATE  ▲ Ages 13-16
// "The Invisible Network"
// Structure is provided — you fill in the logic.
// Be ready to explain every line to a staff member.
// ============================================================
#include <WiFi.h>
#include <WebServer.h>

// TASK: Declare your SSID and password as const char* variables.
// Use your name in the SSID. Same password as everyone else.


// TASK: Declare a WebServer object on port 80.


// TASK: Write a handleRoot() function.
// It should serve an HTML page with:
//   - A proper <!DOCTYPE html> and <html><body> structure
//   - A heading with your name
//   - At least 2 paragraphs of content
//   - Some inline CSS styling (background color, font, etc.)
// Hint: use server.send(200, "text/html", yourHtmlString);


void setup() {
  // TASK: Initialize Serial at 115200 baud

  // TASK: Start the Access Point with your SSID and password

  // TASK: Print the AP IP address to Serial (hint: WiFi.softAPIP())

  // TASK: Register handleRoot for the "/" route

  // TASK: Start the server
}

void loop() {
  // TASK: One line — keep the server running
}

// VERBAL CHECK — explain these to a staff member:
// 1. What is the difference between a client and a server?
// 2. What does WiFi.softAP() actually do vs WiFi.begin()?
// 3. Why does the IP always come up as 192.168.4.1?
// 4. What does server.handleClient() do in loop()?
