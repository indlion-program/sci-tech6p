// ==========================================================
// CAMP CYBER DIVISION - DAY 4 - TEACHER HUB
// The single "scoreboard" board for the whole room.
//
// What this board does:
//   1. Makes ONE WiFi network ("CyberDivision_Hub") on its own subnet
//      (10.0.0.1) so it doesn't clash with the student boards (192.168.4.1).
//   2. Every student board connects to it and reports its status.
//   3. It serves a big-screen DASHBOARD: a grid of codename tiles that
//      glow green (SECURE) and flip to red (HACKED) live during the game.
//
// To use it: connect the projector laptop to the "CyberDivision_Hub" WiFi,
// then open  http://10.0.0.1/  in a browser. That's the whole setup.
//
// Only built-in libraries are used: WiFi, WebServer.
// ==========================================================
#include <WiFi.h>
#include <WebServer.h>
#include <string.h>   // strcmp / strncpy for comparing and copying codenames

// ---- EDIT THESE ----
const char* HUB_SSID        = "CyberDivision_Hub";  // students + projector join this (must match the student sketch)
const int   MAX_CONNECTIONS = 10;                   // most boards the hub will accept (see README - ~10 is the ceiling)
const int   MAX_PLAYERS     = 32;                   // how many codenames the roster can hold
// --------------------
// (This hub network is OPEN - no password - so the projector laptop can join
//  with one tap. It lives in a closed classroom, so that's fine.)

// Give the hub its OWN subnet so it never collides with a student's 192.168.4.1.
IPAddress hubIP(10, 0, 0, 1);
IPAddress hubGateway(10, 0, 0, 1);
IPAddress hubSubnet(255, 255, 255, 0);

WebServer server(80);

// ----------------------------------------------------------
// THE ROSTER - a simple fixed-size list. No dynamic memory needed.
// ----------------------------------------------------------
struct Player {
  char name[24];   // the codename
  bool pwned;      // false = SECURE, true = HACKED
};
Player players[MAX_PLAYERS];
int playerCount = 0;

// Find a player by name. Returns the index, or -1 if not in the roster yet.
int findPlayer(const char* name) {
  for (int i = 0; i < playerCount; i++) {
    if (strcmp(players[i].name, name) == 0) return i;
  }
  return -1;
}

// Add a new player to the roster (if there is room).
void addPlayer(const char* name, bool pwned) {
  if (playerCount >= MAX_PLAYERS) return;            // roster full - ignore extras
  strncpy(players[playerCount].name, name, sizeof(players[0].name) - 1);
  players[playerCount].name[sizeof(players[0].name) - 1] = '\0';   // always null-terminate
  players[playerCount].pwned = pwned;
  playerCount++;
}

// ----------------------------------------------------------
// THE DASHBOARD PAGE
// One raw string literal of plain HTML/CSS/JS. No external libraries and no
// browser storage - it works fully offline. The canvas draws a tile per
// codename and polls /status every 2 seconds to stay live.
// ----------------------------------------------------------
const char DASHBOARD[] = R"DASH(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Camp Cyber Division - Live Board</title>
  <style>
    :root{ --bg:#0a0f0c; --green:#3dffa0; --blue:#60d4ff; --pink:#ff7eb3; --amber:#ffd166; }
    *{ box-sizing:border-box; }
    html,body{ margin:0; height:100%; background:var(--bg); color:var(--green);
               font-family:'Courier New',monospace; overflow:hidden; }
    header{ display:flex; align-items:center; justify-content:space-between;
            padding:14px 26px; border-bottom:2px solid var(--green); }
    h1{ margin:0; font-size:34px; color:var(--blue); text-shadow:0 0 14px var(--blue); letter-spacing:2px; }
    #count{ font-size:30px; color:var(--amber); text-shadow:0 0 10px var(--amber); }
    #reset{ font-size:20px; padding:10px 18px; cursor:pointer; border-radius:10px;
            background:transparent; color:var(--pink); border:2px solid var(--pink);
            text-shadow:0 0 8px var(--pink); font-family:inherit; }
    #reset:hover{ background:var(--pink); color:var(--bg); }
    canvas{ display:block; width:100vw; height:calc(100vh - 72px); }
  </style>
</head>
<body>
  <header>
    <h1>CAMP CYBER DIVISION</h1>
    <div id="count">0 / 0 HACKED</div>
    <button id="reset">RESET BOARD</button>
  </header>
  <canvas id="board"></canvas>

<script>
  var palette = { bg:'#0a0f0c', green:'#3dffa0', blue:'#60d4ff', pink:'#ff7eb3', amber:'#ffd166' };
  var canvas  = document.getElementById('board');
  var ctx     = canvas.getContext('2d');
  var countEl = document.getElementById('count');

  var players  = [];     // [{name, pwned}, ...] from the hub
  var flipTime = {};     // codename -> moment it turned HACKED (drives the flip animation)

  function resize(){
    canvas.width  = canvas.clientWidth;
    canvas.height = canvas.clientHeight;
  }
  window.addEventListener('resize', resize);
  resize();

  // Ask the hub for the latest status every 2 seconds.
  function poll(){
    fetch('/status', {cache:'no-store'})
      .then(function(r){ return r.json(); })
      .then(function(data){
        for (var i = 0; i < data.length; i++){
          var p = data[i];
          if (p.pwned && !flipTime[p.name]) flipTime[p.name] = performance.now(); // just got hacked
          if (!p.pwned) delete flipTime[p.name];                                  // back to secure
        }
        players = data;
      })
      .catch(function(){ /* hub busy - keep showing the last view */ });
  }
  setInterval(poll, 2000);
  poll();

  // Teacher's reset button -> tell the hub to mark everyone SECURE again.
  document.getElementById('reset').addEventListener('click', function(){
    fetch('/reset', {cache:'no-store'}).then(function(){
      flipTime = {};
      poll();
    }).catch(function(){});
  });

  function roundRect(c, x, y, w, h, r){
    c.beginPath();
    c.moveTo(x + r, y);
    c.arcTo(x + w, y,     x + w, y + h, r);
    c.arcTo(x + w, y + h, x,     y + h, r);
    c.arcTo(x,     y + h, x,     y,     r);
    c.arcTo(x,     y,     x + w, y,     r);
    c.closePath();
  }

  function draw(now){
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    var n = players.length;
    var hacked = 0;
    for (var i = 0; i < n; i++){ if (players[i].pwned) hacked++; }
    countEl.textContent = hacked + ' / ' + n + ' HACKED';

    if (n === 0){
      ctx.fillStyle = palette.blue;
      ctx.font = '28px Courier New';
      ctx.textAlign = 'center';
      ctx.textBaseline = 'middle';
      ctx.fillText('Waiting for boards to connect...', canvas.width / 2, canvas.height / 2);
      requestAnimationFrame(draw);
      return;
    }

    // Choose a grid shape that fills the screen nicely.
    var cols = Math.ceil(Math.sqrt(n * canvas.width / canvas.height));
    if (cols < 1) cols = 1;
    var rows = Math.ceil(n / cols);
    var pad  = 16;
    var cw   = canvas.width  / cols;
    var ch   = canvas.height / rows;

    for (var j = 0; j < n; j++){
      var p  = players[j];
      var x  = (j % cols) * cw + pad;
      var y  = Math.floor(j / cols) * ch + pad;
      var w  = cw - pad * 2;
      var h  = ch - pad * 2;

      // When a tile just flipped to HACKED, shake it and flare the glow.
      var glow = 14;
      if (p.pwned && flipTime[p.name]){
        var age = now - flipTime[p.name];
        if (age < 1200){
          var k = 1 - age / 1200;
          x += Math.sin(age / 40) * 10 * k;
          glow = 14 + 26 * k;
        }
      }

      var color = p.pwned ? palette.pink : palette.green;
      ctx.save();
      ctx.shadowColor = color;
      ctx.shadowBlur  = glow;
      ctx.lineWidth   = 3;
      ctx.strokeStyle = color;
      ctx.fillStyle   = p.pwned ? 'rgba(255,126,179,0.18)' : 'rgba(61,255,160,0.08)';
      roundRect(ctx, x, y, w, h, 14);
      ctx.fill();
      ctx.stroke();
      ctx.restore();

      // Codename + status, scaled to fit the tile and readable across a room.
      ctx.fillStyle    = color;
      ctx.textAlign    = 'center';
      ctx.textBaseline = 'middle';

      var nameSize = Math.max(16, Math.min(w / 8, 40));
      ctx.font = 'bold ' + nameSize + 'px Courier New';
      ctx.fillText(p.name, x + w / 2, y + h / 2 - nameSize * 0.5);

      var statusSize = Math.max(14, Math.min(w / 10, 30));
      ctx.font = 'bold ' + statusSize + 'px Courier New';
      ctx.fillText(p.pwned ? 'HACKED' : 'SECURE', x + w / 2, y + h / 2 + nameSize * 0.7);
    }

    requestAnimationFrame(draw);
  }
  requestAnimationFrame(draw);
</script>
</body>
</html>
)DASH";

// ----------------------------------------------------------
// ROUTES
// ----------------------------------------------------------

// GET /register?name=X  ->  add X as SECURE if we haven't seen them.
void handleRegister() {
  String name = server.arg("name");
  if (name.length() == 0) { server.send(400, "text/plain", "need a name"); return; }
  if (findPlayer(name.c_str()) < 0) addPlayer(name.c_str(), false);
  server.send(200, "text/plain", "ok");
}

// GET /pwned?name=X  ->  mark X as HACKED (add them if we never saw a register).
void handlePwned() {
  String name = server.arg("name");
  if (name.length() == 0) { server.send(400, "text/plain", "need a name"); return; }
  int i = findPlayer(name.c_str());
  if (i < 0) addPlayer(name.c_str(), true);
  else       players[i].pwned = true;
  server.send(200, "text/plain", "ok");
}

// GET /status  ->  the roster as a JSON array, e.g.
//   [{"name":"Sarah_Network","pwned":true},{"name":"Max_Byte","pwned":false}]
void handleStatus() {
  String json = "[";
  for (int i = 0; i < playerCount; i++) {
    if (i > 0) json += ",";
    json += "{\"name\":\"";
    json += players[i].name;
    json += "\",\"pwned\":";
    json += players[i].pwned ? "true" : "false";
    json += "}";
  }
  json += "]";
  server.send(200, "application/json", json);
}

// GET /reset  ->  mark everyone SECURE again (the teacher's reset button).
void handleReset() {
  for (int i = 0; i < playerCount; i++) players[i].pwned = false;
  server.send(200, "text/plain", "reset");
}

// GET /  ->  the live dashboard.
void handleRoot() {
  server.send(200, "text/html", DASHBOARD);
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_AP);
  // Put the hub on its own subnet (10.0.0.1) BEFORE starting the AP.
  WiFi.softAPConfig(hubIP, hubGateway, hubSubnet);
  // Open network, channel 1, visible, up to MAX_CONNECTIONS clients.
  WiFi.softAP(HUB_SSID, NULL, 1, 0, MAX_CONNECTIONS);

  Serial.print("Hub network '");
  Serial.print(HUB_SSID);
  Serial.print("' is live at http://");
  Serial.println(WiFi.softAPIP());   // 10.0.0.1

  server.on("/",         handleRoot);
  server.on("/register", handleRegister);
  server.on("/pwned",    handlePwned);
  server.on("/status",   handleStatus);
  server.on("/reset",    handleReset);
  server.begin();

  Serial.println("Dashboard ready.");
  Serial.print("On the projector laptop: join WiFi '");
  Serial.print(HUB_SSID);
  Serial.println("' then open http://10.0.0.1/");
}

void loop() {
  server.handleClient();
}
