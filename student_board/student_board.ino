// ============================================================
// CAMP CYBER DIVISION — STUDENT BOARD  (Explorer)
// Your board hosts a hackable web page. Other campers try to
// breach it. You only touch the two zones near the top.
// ============================================================
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include "mbedtls/base64.h"

// ═══════════════ CHANGE THESE ═══════════════
const char* ssid       = "___";   // codename
const char* password   = "___";   // shared
const char* flag       = "___";   // answer
const char* secretPath = "___";   // path, e.g. /treasure

// ══════════ PASTE YOUR W3SCHOOLS PAGE ══════════
// paste a whole page from w3schools — <html> and <body> included
const char PAGE[] = R"PAGE(
___
)PAGE";

// ═══════════ DON'T TOUCH BELOW THIS LINE ═══════════

// --- The teacher hub (open network on 10.0.0.1). We join it to report in. ---
const char* HUB_SSID = "CyberDivision_Hub";
const char* HUB_URL  = "http://10.0.0.1";

WebServer server(80);
String gPath;                 // your secretPath, cleaned up (always starts with "/")
unsigned long lastBeat = 0;   // heartbeat timer

// Base64-encode a string with the ESP32's built-in mbedtls (no libraries).
String base64Encode(const String& s) {
  unsigned char out[256];
  size_t written = 0;
  if (mbedtls_base64_encode(out, sizeof(out), &written,
        (const unsigned char*)s.c_str(), s.length()) != 0) {
    return String("");        // only if the path is absurdly long
  }
  out[written] = '\0';
  return String((char*)out);
}

// Fire-and-forget GET to the hub. If the hub is down we give up fast so the
// board keeps working on its own.
void hubGet(const String& path) {
  if (WiFi.status() != WL_CONNECTED) return;   // not joined -> skip, no waiting
  HTTPClient http;
  WiFiClient client;
  http.begin(client, String(HUB_URL) + path);
  http.setConnectTimeout(600);
  http.setTimeout(600);
  http.GET();
  http.end();
}

// The submit box shown at the secret path (styled dark-terminal).
const char SUBMIT_PAGE[] = R"HTML(
<!DOCTYPE html><html><head><meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>RESTRICTED</title><style>
  body{background:#0a0f0c;color:#3dffa0;font-family:'Courier New',monospace;text-align:center;padding-top:14vh}
  h1{color:#60d4ff;text-shadow:0 0 12px #60d4ff}
  input{font-size:20px;padding:10px;background:#06120d;color:#3dffa0;border:1px solid #60d4ff;border-radius:8px}
  button{font-size:20px;padding:10px 18px;background:transparent;color:#ffd166;border:1px solid #ffd166;border-radius:8px;cursor:pointer}
</style></head><body>
  <h1>RESTRICTED</h1>
  <p>Enter the flag to breach this board.</p>
  <form method="get"><input name="flag" placeholder="flag" autofocus> <button>SUBMIT</button></form>
</body></html>
)HTML";

// Shown for a wrong flag.
const char DENIED_PAGE[] = R"HTML(
<!DOCTYPE html><html><head><meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ACCESS DENIED</title><style>
  body{background:#0a0f0c;color:#ff5b6e;font-family:'Courier New',monospace;text-align:center;padding-top:14vh}
  h1{font-size:3em;text-shadow:0 0 16px #ff5b6e}
  a{color:#60d4ff}
</style></head><body>
  <h1>ACCESS DENIED</h1>
  <p><a href="?">try again</a></p>
</body></html>
)HTML";

// Shown for the correct flag. {{name}} is swapped for the victim's codename.
const char PWNED_PAGE[] = R"HTML(
<!DOCTYPE html><html><head><meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>PWNED</title><style>
  body{background:#0a0f0c;color:#3dffa0;font-family:'Courier New',monospace;text-align:center;padding-top:12vh}
  h1{font-size:3.2em;color:#ff5b6e;text-shadow:0 0 20px #ff5b6e;margin:0}
  p{font-size:1.4em;color:#ffd166}
</style></head><body>
  <h1>YOU PWNED {{name}}</h1>
  <p>Breach reported. Tell a teacher you scored one.</p>
</body></html>
)HTML";

// GET /  ->  the camper's pasted page, PLUS the header that points to the
// secret path (base64-encoded). The camper never writes this header.
void handleRoot() {
  server.sendHeader("X-Access", base64Encode(gPath));
  server.send(200, "text/html", PAGE);
}

// GET <secretPath>  ->  submit box, or check the flag.
void handleSecret() {
  if (!server.hasArg("flag")) {              // no flag yet -> show the box
    server.send(200, "text/html", SUBMIT_PAGE);
    return;
  }
  if (server.arg("flag") == String(flag)) {  // correct -> report + celebrate
    hubGet("/breach?name=" + String(ssid));
    String page = PWNED_PAGE;
    page.replace("{{name}}", ssid);
    server.send(200, "text/html", page);
  } else {                                   // wrong -> denied
    server.send(200, "text/html", DENIED_PAGE);
  }
}

void setup() {
  Serial.begin(115200);

  // Make sure the secret path starts with a "/".
  gPath = secretPath;
  if (!gPath.startsWith("/")) gPath = "/" + gPath;

  // Be our own AP (for attackers) AND a station on the hub (to report in).
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid, password);          // your network, default IP 192.168.4.1
  WiFi.begin(HUB_SSID);                  // join the open teacher hub

  server.on("/", handleRoot);
  server.on(gPath, handleSecret);        // your secret page
  server.begin();

  Serial.print("Board '"); Serial.print(ssid);
  Serial.print("' live at "); Serial.println(WiFi.softAPIP());   // 192.168.4.1
}

void loop() {
  server.handleClient();

  // Heartbeat: tell the hub we're alive every ~5s (best-effort).
  if (millis() - lastBeat > 5000) {
    lastBeat = millis();
    hubGet("/register?name=" + String(ssid));
  }
}
