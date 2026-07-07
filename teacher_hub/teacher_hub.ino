// ============================================================
// CAMP CYBER DIVISION — TEACHER HUB (dashboard)
// One board for the whole room. Every student board joins this
// open network and reports in. Put it on the projector:
//   join Wi-Fi "CyberDivision_Hub" -> open http://10.0.0.1/
// ============================================================
#include <WiFi.h>
#include <WebServer.h>
#include <string.h>   // strcmp / strncpy for codenames

// ---- settings ----
const char* HUB_SSID        = "CyberDivision_Hub";   // students + projector join this (open)
const int   MAX_CONNECTIONS = 10;                    // AP client ceiling
const int   MAX_BOARDS      = 40;                    // roster size
const unsigned long STALE_MS = 15000;                // drop a board not seen in ~15s

IPAddress hubIP(10, 0, 0, 1);
IPAddress hubGateway(10, 0, 0, 1);
IPAddress hubSubnet(255, 255, 255, 0);

WebServer server(80);

struct Board {
  char name[24];
  bool breached;
  unsigned long lastSeen;
};
Board boards[MAX_BOARDS];
int boardCount = 0;

int findBoard(const char* name) {
  for (int i = 0; i < boardCount; i++) {
    if (strcmp(boards[i].name, name) == 0) return i;
  }
  return -1;
}

// Add or update a board (upsert). Keeps its breached flag.
void upsert(const char* name, unsigned long now) {
  int i = findBoard(name);
  if (i < 0) {
    if (boardCount >= MAX_BOARDS) return;
    i = boardCount++;
    strncpy(boards[i].name, name, sizeof(boards[i].name) - 1);
    boards[i].name[sizeof(boards[i].name) - 1] = '\0';
    boards[i].breached = false;
  }
  boards[i].lastSeen = now;   // heartbeat: never clears breached
}

// ---- dashboard page (self-contained: no CDN, no browser storage) ----
const char DASHBOARD[] = R"DASH(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Camp Cyber Division — Live Board</title>
  <style>
    :root{ --bg:#0a0f0c; --green:#3dffa0; --blue:#60d4ff; --red:#ff5b6e; --amber:#ffd166; }
    *{ box-sizing:border-box; }
    /* the id rules below set display:flex/grid, which would beat the browser's
       [hidden] rule — force hidden to win so START can hide the splash */
    [hidden]{ display:none !important; }
    html,body{ margin:0; height:100%; background:var(--bg); color:var(--green);
               font-family:'Courier New',monospace; overflow:hidden; }
    #splash{ position:fixed; inset:0; background:var(--bg); z-index:40;
             display:flex; flex-direction:column; align-items:center; justify-content:center; }
    #splash h1{ font-size:54px; color:var(--blue); text-shadow:0 0 18px var(--blue); letter-spacing:3px; }
    #splash p{ font-size:22px; color:var(--amber); }
    #arm{ margin-top:24px; font-size:30px; padding:18px 46px; cursor:pointer; border-radius:14px;
          background:transparent; color:var(--green); border:3px solid var(--green);
          text-shadow:0 0 10px var(--green); font-family:inherit; }
    #arm:hover{ background:var(--green); color:var(--bg); }
    header{ display:flex; align-items:center; justify-content:space-between;
            padding:16px 30px; border-bottom:2px solid var(--green); }
    h2{ margin:0; font-size:38px; color:var(--blue); text-shadow:0 0 14px var(--blue); letter-spacing:2px; }
    #count{ font-size:34px; color:var(--amber); text-shadow:0 0 10px var(--amber); }
    #reset{ font-size:22px; padding:12px 22px; cursor:pointer; border-radius:12px;
            background:transparent; color:var(--red); border:2px solid var(--red);
            text-shadow:0 0 8px var(--red); font-family:inherit; }
    #reset:hover{ background:var(--red); color:var(--bg); }
    #grid{ display:grid; grid-template-columns:repeat(auto-fill, minmax(260px, 1fr));
           gap:22px; padding:30px; height:calc(100vh - 76px); overflow:auto; align-content:start; }
    .tile{ border:3px solid var(--green); border-radius:16px; padding:26px 18px; text-align:center;
           box-shadow:0 0 18px rgba(61,255,160,.25); transition:all .3s; }
    .tile .name{ font-size:40px; font-weight:bold; color:var(--green); word-break:break-word; }
    .tile .status{ margin-top:12px; font-size:24px; color:var(--amber); letter-spacing:2px; }
    .tile.breached{ border-color:var(--red); box-shadow:0 0 26px rgba(255,91,110,.6);
                    background:rgba(255,91,110,.12); }
    .tile.breached .name{ color:var(--red); }
    .tile.breached .status{ color:var(--red); }
    #empty{ grid-column:1/-1; text-align:center; font-size:26px; color:var(--blue); margin-top:12vh; }
    #matrix{ position:fixed; inset:0; z-index:20; background:#000; }
    #victim{ position:fixed; top:6vh; left:0; right:0; z-index:21; text-align:center;
             font-size:60px; font-weight:bold; color:var(--red); text-shadow:0 0 22px var(--red);
             letter-spacing:3px; }
  </style>
</head>
<body>
  <div id="splash">
    <h1>CAMP CYBER DIVISION</h1>
    <p>Tap to arm the live board (turns on sound).</p>
    <button id="arm">START</button>
  </div>

  <header hidden id="bar">
    <h2>LIVE BOARD</h2>
    <div id="count">0 / 0 BREACHED</div>
    <button id="reset">RESET</button>
  </header>
  <main id="grid" hidden></main>

  <canvas id="matrix" hidden></canvas>
  <div id="victim" hidden></div>

<script>
  var actx = null;                 // audio context (unlocked by the START tap)
  var tiles = {};                  // name -> tile element
  var breachedState = {};          // name -> was it breached last poll?
  var grid = document.getElementById('grid');
  var bar = document.getElementById('bar');
  var splash = document.getElementById('splash');
  var countEl = document.getElementById('count');
  var mcanvas = document.getElementById('matrix');
  var mctx = mcanvas.getContext('2d');
  var victimEl = document.getElementById('victim');

  // ---- START splash: unlock audio, then begin polling ----
  document.getElementById('arm').addEventListener('click', function(){
    try{
      actx = new (window.AudioContext || window.webkitAudioContext)();
      if (actx.state === 'suspended') actx.resume();
    }catch(e){ actx = null; }
    splash.hidden = true; bar.hidden = false; grid.hidden = false;
    poll();
    setInterval(poll, 2000);
  });

  document.getElementById('reset').addEventListener('click', function(){
    fetch('/reset', {cache:'no-store'}).then(poll).catch(function(){});
  });

  // ---- alarm sound (needs the unlocked audio context) ----
  function alarm(){
    if (!actx) return;
    var o = actx.createOscillator(), g = actx.createGain();
    o.type = 'square';
    o.connect(g); g.connect(actx.destination);
    var t = actx.currentTime;
    o.frequency.setValueAtTime(880, t);
    o.frequency.linearRampToValueAtTime(180, t + 0.45);
    g.gain.setValueAtTime(0.0001, t);
    g.gain.exponentialRampToValueAtTime(0.35, t + 0.03);
    g.gain.exponentialRampToValueAtTime(0.0001, t + 0.5);
    o.start(t); o.stop(t + 0.5);
  }

  // ---- 5-second Matrix rain of 1s and 0s (green + red) ----
  var matrixRAF = null, matrixTimer = null, drops = [], cell = 24;
  function startMatrix(name){
    mcanvas.width = window.innerWidth;
    mcanvas.height = window.innerHeight;
    victimEl.textContent = 'BREACHED: ' + name;
    victimEl.hidden = false;
    mcanvas.hidden = false;
    mctx.fillStyle = '#000';
    mctx.fillRect(0, 0, mcanvas.width, mcanvas.height);
    var cols = Math.floor(mcanvas.width / cell) + 1;
    drops = [];
    for (var c = 0; c < cols; c++) drops[c] = Math.floor(Math.random() * (mcanvas.height / cell));

    function step(){
      mctx.fillStyle = 'rgba(0,0,0,0.14)';
      mctx.fillRect(0, 0, mcanvas.width, mcanvas.height);
      mctx.font = cell + 'px monospace';
      for (var i = 0; i < drops.length; i++){
        var ch = Math.random() < 0.5 ? '0' : '1';
        mctx.fillStyle = Math.random() < 0.18 ? '#ff5b6e' : '#3dffa0';
        mctx.fillText(ch, i * cell, drops[i] * cell);
        if (drops[i] * cell > mcanvas.height && Math.random() > 0.975) drops[i] = 0;
        drops[i]++;
      }
      matrixRAF = requestAnimationFrame(step);
    }
    cancelAnimationFrame(matrixRAF);
    step();
    clearTimeout(matrixTimer);
    matrixTimer = setTimeout(stopMatrix, 5000);
  }
  function stopMatrix(){
    cancelAnimationFrame(matrixRAF); matrixRAF = null;
    mcanvas.hidden = true; victimEl.hidden = true;
  }

  function makeTile(name){
    var d = document.createElement('div');
    d.className = 'tile';
    var n = document.createElement('div'); n.className = 'name'; n.textContent = name;
    var s = document.createElement('div'); s.className = 'status'; s.textContent = 'READY';
    d.appendChild(n); d.appendChild(s);
    return d;
  }

  // ---- poll the hub, rebuild tiles keyed by name ----
  function poll(){
    fetch('/status', {cache:'no-store'})
      .then(function(r){ return r.json(); })
      .then(function(data){
        var present = {}, breachCount = 0;
        for (var i = 0; i < data.length; i++){
          var p = data[i];
          present[p.name] = true;
          if (p.breached) breachCount++;
          var el = tiles[p.name];
          if (!el){
            el = makeTile(p.name);
            tiles[p.name] = el; grid.appendChild(el);
            breachedState[p.name] = p.breached;      // no effect on first sight
          } else if (p.breached && !breachedState[p.name]){
            alarm(); startMatrix(p.name);            // newly breached -> effect
          }
          breachedState[p.name] = p.breached;
          el.className = p.breached ? 'tile breached' : 'tile';
          el.querySelector('.status').textContent = p.breached ? 'BREACHED' : 'READY';
        }
        // drop tiles for boards that went stale (pruned by the hub)
        for (var name in tiles){
          if (!present[name]){
            grid.removeChild(tiles[name]);
            delete tiles[name]; delete breachedState[name];
          }
        }
        countEl.textContent = breachCount + ' / ' + data.length + ' BREACHED';
        var empty = document.getElementById('empty');
        if (data.length === 0 && !empty){
          empty = document.createElement('div'); empty.id = 'empty';
          empty.textContent = 'Waiting for boards to power on...';
          grid.appendChild(empty);
        } else if (data.length > 0 && empty){
          empty.parentNode.removeChild(empty);
        }
      })
      .catch(function(){ /* hub busy - keep last view */ });
  }
</script>
</body>
</html>
)DASH";

// ---- routes ----
void handleRegister() {
  String name = server.arg("name");
  if (name.length() == 0) { server.send(400, "text/plain", "need name"); return; }
  upsert(name.c_str(), millis());
  server.send(200, "text/plain", "ok");
}

void handleBreach() {
  String name = server.arg("name");
  if (name.length() == 0) { server.send(400, "text/plain", "need name"); return; }
  upsert(name.c_str(), millis());
  int i = findBoard(name.c_str());
  if (i >= 0) boards[i].breached = true;
  server.send(200, "text/plain", "ok");
}

// JSON of boards seen within STALE_MS, e.g. [{"name":"Sarah","breached":true}]
void handleStatus() {
  unsigned long now = millis();
  String json = "[";
  bool first = true;
  for (int i = 0; i < boardCount; i++) {
    if (now - boards[i].lastSeen > STALE_MS) continue;   // prune stale
    if (!first) json += ",";
    first = false;
    json += "{\"name\":\"";
    json += boards[i].name;
    json += "\",\"breached\":";
    json += boards[i].breached ? "true" : "false";
    json += "}";
  }
  json += "]";
  server.send(200, "application/json", json);
}

void handleReset() {
  boardCount = 0;
  server.send(200, "text/plain", "reset");
}

void handleRoot() {
  server.send(200, "text/html", DASHBOARD);
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(hubIP, hubGateway, hubSubnet);        // 10.0.0.1 subnet (no clash)
  WiFi.softAP(HUB_SSID, NULL, 1, 0, MAX_CONNECTIONS);     // open AP, up to 10 clients

  server.on("/",         handleRoot);
  server.on("/register", handleRegister);
  server.on("/breach",   handleBreach);
  server.on("/status",   handleStatus);
  server.on("/reset",    handleReset);
  server.begin();

  Serial.print("Hub live. Join '"); Serial.print(HUB_SSID);
  Serial.print("' then open http://"); Serial.println(WiFi.softAPIP());   // 10.0.0.1
}

void loop() {
  server.handleClient();
}
