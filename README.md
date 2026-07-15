# Camp Cyber Division — Explorer

A fun-first, hands-on cybersecurity camp for kids. Every camper flashes **one
ESP32 board** that hosts a small hackable website. Campers hide a flag on their
own board, then crack each other's boards by following a real investigation
trail — **hidden flag → custom header → base64 → secret page**. A single
**teacher hub** board runs a live projector dashboard that lights up (Matrix
rain + alarm) whenever a board gets breached.

Single track, called **Explorer**. WiFi only — no Bluetooth, no LED, no extra
parts. Everything runs offline.

---

## The win chain

```
   join a board's WiFi (one shared password)
   └─▶ find the flag hidden in the page source   (camper chose the trick)
       └─▶ Network tab: read header  X-Access: <base64>
           └─▶ decode base64  →  a secret path, e.g. /treasure
               └─▶ open that path → submit box → enter the flag
                   ├─ correct → "YOU PWNED <name>"  + dashboard marks breached
                   └─ wrong   → "ACCESS DENIED"
```

- **Base64 is used only on the path** in the `X-Access` header. The flag itself
  is plain text.
- The secret path is **per-camper** — they set it in the edit zone; the board
  base64-encodes it into the header automatically. The camper never writes the
  header.
- The only "breach" is a board **reporting itself** pwned. No deauth, no
  attacking anyone else's real network.

---

## Files

```
student_board/student_board.ino   One sketch per camper. AP + STA.
teacher_hub/teacher_hub.ino       One hub board = the live dashboard.
slides/camp_slides.html           4-day slide deck (arrow keys / click to move).
tools/base64_encoder.html         Offline base64 encode/decode helper.
installers/                       Arduino IDE, USB drivers, arduino-cli (Windows).
SETUP.bat / SETUP.ps1             One-click Windows setup for a camper laptop.
README.txt                        Short camper-facing setup guide.
pentest/esp32_ble_just_works/     BLE "Just Works" pairing tester (lab use, see its README).
```

## Hardware

- **ESP32 Dev Module** (ESP-WROOM-32), Arduino core.
- A **data-capable USB cable** (charge-only cables can't flash).
- Nothing else — no LED, no sensors, no breadboard.

---

## The student board — what a camper edits

Open `student_board/student_board.ino`. A camper only touches the two fenced
zones at the top:

```cpp
// ═══════════════ CHANGE THESE ═══════════════
ssid       = "___"   // codename
password   = "___"   // shared
flag       = "___"   // answer
secretPath = "___"   // path, e.g. /treasure

// ══════════ PASTE YOUR W3SCHOOLS PAGE ══════════
R"PAGE(
___
)PAGE"
// ═══════════ DON'T TOUCH BELOW THIS LINE ═══════════
```

Because the camper owns the **whole** `/` page, pasting a complete W3Schools
document (with `<html>` and `<body>`) is fine — the mechanic pages (the secret
path and the header) are separate routes and are never injected into their page,
so there's no way to double up `<html>`/`<body>`.

Everything below the fence is automatic: start the AP, join the hub, serve the
page with the `X-Access` header, run the secret-path submit page, and heartbeat
the hub. Built-in libraries only: `WiFi.h`, `WebServer.h`, `HTTPClient.h`,
`mbedtls/base64.h`.

---

## Running the game

1. **Flash the hub.** Put `teacher_hub/teacher_hub.ino` on one board. It creates
   an open WiFi network **`CyberDivision_Hub`** on `10.0.0.1`.
2. **Open the dashboard.** On the projector laptop, join `CyberDivision_Hub` and
   open **`http://10.0.0.1/`**. Tap **START** once (this turns on the sound).
3. **Flash the student boards.** Each camper edits their zones and uploads. Their
   board appears on the dashboard as **READY** within a few seconds.
4. **Play.** Campers crack each other's boards. A breach triggers a 5-second
   Matrix effect + alarm, then that tile flips to **BREACHED**.
5. **Reset** any time with the dashboard's RESET button.

### The 4-day plan (see the slide deck)

| Day | Theme | What campers do |
|----|-------|-----------------|
| 1 | Get it running | Flash the board, open `192.168.4.1`, meet the Network tab |
| 2 | Make it yours | Paste a whole W3Schools page into the PASTE slot; reflash |
| 3 | Secrets & communication | Set `secretPath`, hide the flag in source, learn the header→base64→path trail |
| 4 | Play | Attack the room; watch the live dashboard |

---

## Known limits & gotchas

- **Subnet:** a SoftAP defaults to `192.168.4.1`. The hub is moved to its own
  subnet **`10.0.0.1`** (`softAPConfig`) so it never clashes with the students'
  boards, which keep `192.168.4.1`.
- **~10 boards per hub:** the hub uses `max_connection = 10`. About **10 student
  boards + the projector** is the practical ceiling — run a second hub (different
  SSID) for a bigger group.
- **Shared channel:** in AP+STA the ESP32 forces its own AP onto the hub's
  channel automatically. The student sketch never hard-codes a channel.
- **Best-effort reporting:** heartbeats/breach reports use short timeouts and are
  skipped if the hub isn't reachable. **Pull the hub's power and every student
  board keeps working** — only the live scoreboard pauses.
- **Open networks by design:** the hub is open so the projector joins in one tap;
  each student AP uses the one shared password the teacher hands out. It's a
  closed classroom game.

## Ethics

The only "ban" is a board dropping/reporting **itself**. Cracking a puzzle just
means reading the HTML and headers a board *chose* to send you — the same tools
professionals use (Network tab, headers, base64, DevTools), used for good.

---

## Building

Both sketches target **`esp32:esp32:esp32`** (ESP32 Dev Module) and use only
built-in libraries, so they compile with a stock ESP32 Arduino core — no extra
library installs.
