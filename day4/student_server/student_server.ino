// ==========================================================
// CAMP CYBER DIVISION - DAY 4 - STUDENT BOARD
// "Defend your site. Crack everyone else's."
//
// What this board does:
//   1. Makes its OWN open WiFi network (named after your codename).
//   2. Serves a puzzle web page that hides a secret ADMIN CODE.
//   3. At the same time it quietly joins the TEACHER HUB and
//      reports "I'm in the game" so your tile shows on the big screen.
//   4. If an attacker finds your code and submits it:
//        - your LED turns ON,
//        - your board tells the hub "I got hacked!",
//        - then your board kicks everyone off YOUR OWN network.
//      Press the EN button on the board to rejoin the game.
//
// This board uses AP+STA mode:
//   AP  = it is its own access point (the puzzle network).
//   STA = it is also a "station" connected to the teacher hub.
//
// Only built-in libraries are used: WiFi, WebServer, HTTPClient.
// ==========================================================
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

// ---- EDIT THESE ----
const char* CODENAME = "Sarah_Network";   // your spy name = your WiFi name. Letters/numbers/underscores only!
const char* SECRET   = "RED-FALCON-42";   // the admin code attackers must find. Keep it secret... until they crack it!
const int   LED      = 23;                 // LED pin: GPIO23 -> 220 ohm resistor -> LED(+) -> LED(-) -> GND

const char* HUB_SSID = "CyberDivision_Hub"; // must match the teacher hub's WiFi name
const char* HUB_IP   = "10.0.0.1";          // the teacher hub's address (do not change unless the teacher did)
// --------------------

WebServer server(80);   // the puzzle web server lives on port 80

// These flags let us do the "kick everyone off" step AFTER the
// attacker has actually seen the "PWNED" page (see loop() below).
bool captured      = false;   // has someone entered the correct code?
bool reportedToHub = false;   // have we told the hub we were hacked yet?
bool apIsUp        = true;     // is our puzzle network still running?
unsigned long capturedAtMs = 0;
const unsigned long KICK_AFTER_MS = 2500;  // wait 2.5s so they can read "PWNED" before we kick them

// ----------------------------------------------------------
// THE PUZZLE PAGE
// This is ONE raw string literal - everything between R"PAGE(  and  )PAGE"
// is plain HTML. You can paste HTML copied straight from W3Schools in the
// PASTE AREA below, quotes and all, with NO escaping needed. The only thing
// you must never type inside is the closing marker  )PAGE"  itself.
//
// {{SECRET}} is a placeholder. The code fills it in with your SECRET above,
// so the hidden ADMIN CODE comment always matches - you only set the code once.
// ----------------------------------------------------------
const char PUZZLE_PAGE[] = R"PAGE(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>:: SECURE TERMINAL ::</title>
  <style>
    :root{ --bg:#0a0f0c; --green:#3dffa0; --blue:#60d4ff; --pink:#ff7eb3; --amber:#ffd166; }
    body{ background:var(--bg); color:var(--green); font-family:'Courier New',monospace; padding:24px; }
    h1{ color:var(--blue); text-shadow:0 0 12px var(--blue); }
    .card{ border:1px solid var(--green); border-radius:12px; padding:20px; max-width:640px;
           box-shadow:0 0 18px rgba(61,255,160,.25); }
    .hint{ color:var(--pink); }
    input,button{ font-size:18px; padding:10px; border-radius:8px; margin-top:8px; }
    input{ background:#06120d; color:var(--green); border:1px solid var(--blue); }
    button{ background:transparent; color:var(--amber); border:1px solid var(--amber); cursor:pointer; }
    button:hover{ background:var(--amber); color:var(--bg); }
  </style>
</head>
<body>

  <!-- ADMIN CODE: {{SECRET}} -->

  <div class="card">
    <h1>SYSTEM LOCKED</h1>
    <p>This terminal belongs to an agent. Only the admin code unlocks it.</p>
    <p class="hint">Tip for hackers: the answer is closer than it looks. Try "View Page Source".</p>

    <!-- ===================== PASTE AREA START ===================== -->
    <!-- Campers: paste your own HTML from W3Schools here to dress up    -->
    <!-- your page (a story, an image, colours...). No escaping needed!  -->

    <p>Welcome to my server. You'll never break in. ;)</p>

    <!-- ====================== PASTE AREA END ====================== -->

    <h3>Enter admin code:</h3>
    <form action="/admin" method="get">
      <input type="text" name="code" placeholder="ADMIN CODE">
      <button type="submit">UNLOCK</button>
    </form>
  </div>

</body>
</html>
)PAGE";

// ----------------------------------------------------------
// THE "YOU GOT PWNED" PAGE (shown to the attacker after a correct code)
// ----------------------------------------------------------
const char PWNED_PAGE[] = R"PAGE(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>PWNED</title>
  <style>
    :root{ --bg:#0a0f0c; --pink:#ff7eb3; --amber:#ffd166; }
    body{ background:var(--bg); color:var(--pink); font-family:'Courier New',monospace;
          text-align:center; padding-top:12vh; }
    h1{ font-size:5em; text-shadow:0 0 20px var(--pink); margin:0; }
    p{ font-size:1.5em; color:var(--amber); }
  </style>
</head>
<body>
  <h1>PWNED!</h1>
  <p>You cracked this terminal. Nicely done, agent.</p>
  <p>This network is shutting down now. Tell a teacher you scored a hack!</p>
</body>
</html>
)PAGE";

// ----------------------------------------------------------
// THE "WRONG CODE" PAGE
// ----------------------------------------------------------
const char WRONG_PAGE[] = R"PAGE(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ACCESS DENIED</title>
  <style>
    body{ background:#0a0f0c; color:#ff7eb3; font-family:'Courier New',monospace;
          text-align:center; padding-top:12vh; }
    a{ color:#60d4ff; }
  </style>
</head>
<body>
  <h1>ACCESS DENIED</h1>
  <p>Wrong code. Keep digging, agent.</p>
  <p><a href="/">&laquo; back to the terminal</a></p>
</body>
</html>
)PAGE";

// ----------------------------------------------------------
// Best-effort message to the teacher hub.
// "Best-effort" means: if the hub is off or busy, we give up FAST and the
// puzzle keeps working. We never let hub problems freeze our web server.
// ----------------------------------------------------------
void tellHub(String path) {
  if (WiFi.status() != WL_CONNECTED) return;   // not joined to the hub -> skip, no problem

  HTTPClient http;
  WiFiClient client;
  String url = "http://" + String(HUB_IP) + path;
  http.begin(client, url);
  http.setConnectTimeout(1500);   // give up connecting after 1.5s
  http.setTimeout(1500);          // give up waiting for a reply after 1.5s
  http.GET();                     // we don't care what it returns
  http.end();
}

// GET /  ->  show the puzzle page (with the real secret filled in)
void handleRoot() {
  String page = PUZZLE_PAGE;          // copy the template into an editable String
  page.replace("{{SECRET}}", SECRET); // drop in your admin code so the hidden comment matches
  server.send(200, "text/html", page);
}

// GET /admin?code=...  ->  check the code
void handleAdmin() {
  if (server.arg("code") == SECRET) {
    // Correct! Light the LED right away.
    digitalWrite(LED, HIGH);

    // Send the PWNED page FIRST, while the attacker is still connected.
    server.send(200, "text/html", PWNED_PAGE);

    // Then remember to report + kick (done in loop, so the page sends cleanly).
    captured      = true;
    reportedToHub = false;
    capturedAtMs  = millis();
  } else {
    server.send(200, "text/html", WRONG_PAGE);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);   // LED starts OFF (board not yet cracked)

  // AP+STA: be our own access point AND a station on the hub at the same time.
  WiFi.mode(WIFI_AP_STA);

  // 1) Start our OWN puzzle network (open = no password, default IP 192.168.4.1).
  WiFi.softAP(CODENAME);
  Serial.print("Puzzle network '");
  Serial.print(CODENAME);
  Serial.print("' is live at http://");
  Serial.println(WiFi.softAPIP());   // 192.168.4.1

  // 2) Start the puzzle web server so attackers can reach us immediately.
  server.on("/", handleRoot);
  server.on("/admin", handleAdmin);
  server.begin();

  // 3) Join the teacher hub (best-effort) and announce ourselves.
  //    In AP+STA the ESP32 automatically uses the hub's WiFi channel, so we
  //    do NOT set a channel ourselves (setting one could break the hub link).
  Serial.print("Looking for the hub '");
  Serial.print(HUB_SSID);
  Serial.println("'...");
  WiFi.begin(HUB_SSID);                            // open hub -> no password needed
  for (int i = 0; i < 16 && WiFi.status() != WL_CONNECTED; i++) {
    delay(500);            // wait up to ~8 seconds for the hub (only at boot)
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Joined the hub. My hub IP is ");
    Serial.println(WiFi.localIP());
    tellHub("/register?name=" + String(CODENAME));   // show up on the dashboard as SECURE
  } else {
    Serial.println("No hub found - playing standalone. The puzzle still works fine.");
  }
}

void loop() {
  server.handleClient();   // keep serving the puzzle to attackers

  // After a successful hack, do the slow stuff here so the PWNED page
  // had time to reach the attacker first.
  if (captured && !reportedToHub) {
    tellHub("/pwned?name=" + String(CODENAME));   // tell the big screen we got hacked
    reportedToHub = true;
  }

  if (captured && apIsUp && (millis() - capturedAtMs > KICK_AFTER_MS)) {
    WiFi.softAPdisconnect(true);   // shut down OUR OWN network -> kicks every attacker off
    apIsUp = false;
    Serial.println("Network shut down. Press the EN button to rejoin the game.");
  }
}
