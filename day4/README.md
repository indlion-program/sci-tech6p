# Camp Cyber Division — Day 4: The Live CTF Arena

On the last day, **every camper's ESP32 becomes a tiny hackable website.** Each
board hides a secret *admin code*. Campers crack each other's boards, and a
single **teacher hub** shows the whole room on a projector: green tiles for
boards that are still **SECURE**, red **HACKED** tiles for ones that have been
cracked — flipping live as the game happens.

```
   STUDENT BOARDS (one per camper)            TEACHER HUB (one board)
   +------------------------+                 +-----------------------------+
   | own open WiFi:         |   reports to    | WiFi: CyberDivision_Hub     |
   | "Sarah_Network"        | --------------> | IP:   10.0.0.1              |
   | puzzle page @          |   /register     | live dashboard @ http://    |
   | 192.168.4.1            |   /pwned        | 10.0.0.1/  (open in browser)|
   | LED on GPIO23          |                 | grid of green/red tiles     |
   +------------------------+                 +-----------------------------+
        ^   attackers connect here                  ^  projector laptop joins here
```

Everything runs over **WiFi only** — no Bluetooth. (Bluetooth + WiFi together is
too memory-heavy for the ESP32.) Each student board runs **AP+STA**: it is its
*own* access point (the puzzle network) *and* a station joined to the hub at the
same time.

---

## 1. What you need per board

- **ESP32 Dev Module** (ESP-WROOM-32).
- **One LED + one 220 Ω resistor** per student board (the hub needs no LED).
- A data-capable USB cable (charge-only cables will not flash).

### LED wiring (student boards only)

```
   GPIO23 ----[ 220 Ω ]----|>|---- GND
                          LED
                     (long leg = +, the side toward GPIO23)
```

- **GPIO23** → one leg of the **220 Ω** resistor.
- Resistor's other leg → LED **anode** (long leg, +).
- LED **cathode** (short leg, −) → **GND**.

When a camper's code is cracked, GPIO23 goes HIGH and the LED lights up.

---

## 2. One-time computer setup (Arduino IDE)

You only do this once per laptop.

1. Install the **Arduino IDE 2.x** (it's in this camp's `installers/` folder, or
   from arduino.cc).
2. **Add the ESP32 boards URL:** *File → Preferences →
   "Additional boards manager URLs"* and paste:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. **Install the ESP32 core:** *Tools → Board → Boards Manager…*, search
   **esp32**, install **"esp32 by Espressif Systems."**
4. **Select the board:** *Tools → Board → ESP32 Arduino → **ESP32 Dev Module***.
5. **Pick the port:** plug in the board, then *Tools → Port → COMx* (Windows) or
   `/dev/ttyUSB0` (Linux/Mac). The port number can change each time you plug in.

> If no port appears, the USB cable is probably charge-only — swap it for a data
> cable, or (re)install the CH340 / CP2102 USB driver from `installers/`.

---

## 3. Flashing the sketches

**Important:** an Arduino `.ino` file must live in a folder of the **same name**.
So always copy the **whole folder**, not just the `.ino`.

```
day4/
  student_server/student_server.ino   <- flash onto every camper board
  teacher_hub/teacher_hub.ino         <- flash onto ONE hub board
```

Distribute the whole `day4/` folder on a USB stick or pre-loaded sketchbook.

### Flash the hub (do this first)

1. Open `teacher_hub/teacher_hub.ino`.
2. (Optional) edit the **`// ---- EDIT THESE ----`** block — the WiFi name, max
   connections, roster size.
3. Upload. Open *Tools → Serial Monitor* at **115200 baud**; you should see
   `Hub network 'CyberDivision_Hub' is live at http://10.0.0.1`.

### Flash each student board

1. Open `student_server/student_server.ino`.
2. Edit the **`// ---- EDIT THESE ----`** block at the very top:
   - `CODENAME` — the camper's spy name; this is also their WiFi name.
     Use **letters, numbers and underscores only** (e.g. `Sarah_Network`).
   - `SECRET` — the admin code attackers must find (e.g. `RED-FALCON-42`).
   - `LED` — leave at **23** unless you wired a different pin.
   - `HUB_SSID` / `HUB_IP` — must match the hub (defaults already do).
3. Upload. Done — that board is now in the game.

> **Tip:** if upload fails with "Failed to connect," hold the **BOOT** button on
> the ESP32 while clicking Upload, and release once "Connecting…" appears.

---

## 4. How a camper builds their puzzle

Open `student_server/student_server.ino` and look at the puzzle page — it's
**one big raw string** between `R"PAGE(` and `)PAGE"`, so it's just plain HTML:

- The hidden flag is in an HTML comment: `<!-- ADMIN CODE: {{SECRET}} -->`.
  `{{SECRET}}` is auto-filled with the `SECRET` you set up top, so you only
  write the code **once** and the hidden comment always matches.
- There is a clearly marked **PASTE AREA** where campers can drop in HTML copied
  straight from W3Schools — **quotes and all, no escaping needed**. (The only
  thing you must never type inside the page is the closing marker `)PAGE"`.)

**How attackers crack it:** connect to the camper's WiFi → open `192.168.4.1` →
**View Page Source** → read the hidden `ADMIN CODE` → type it into the form. A
correct code lights the LED, reports the hack to the hub, and the board then
drops its own network for a few seconds (kicking everyone off). Press the **EN**
button on the board to rejoin the game.

---

## 5. Running the game

1. **Power the hub.** Leave it running.
2. **On the projector laptop:** join the WiFi **`CyberDivision_Hub`** (it's open,
   no password), then open **`http://10.0.0.1/`** in a browser. You'll see the
   dashboard. Press **F11** for full screen.
3. **Power on the student boards.** Each one appears on the dashboard as a green
   **SECURE** tile within a couple of seconds.
4. **Play.** Campers hop onto each other's WiFi networks and crack the puzzles.
   When a board is cracked its tile flips to red **HACKED** with a little shake,
   and the live counter at the top ticks up.
5. **Reset between rounds:** click **RESET BOARD** on the dashboard to mark every
   tile SECURE again. (Boards whose network was kicked off rejoin when a camper
   presses **EN**.)

---

## 6. Known limits & gotchas (read this)

- **Subnet clash (handled):** a SoftAP defaults to `192.168.4.1`. If the hub used
  that too, it would collide with the student boards. The hub therefore runs on
  its **own subnet, `10.0.0.1`** (`softAPConfig`). Student boards keep the
  default `192.168.4.1` for their own network and report to `http://10.0.0.1/`.
- **One shared channel (handled):** in AP+STA the ESP32 forces its own access
  point onto the hub's WiFi channel. This is automatic — the student sketch does
  **not** hard-code a channel, so don't add one.
- **~10 boards per hub:** the hub is set to `max_connection = 10`. With roughly
  **10 student boards + the projector browser** you're at the practical ceiling.
  For a bigger group, run a **second hub** (give it a different `HUB_SSID`) and
  split the campers across the two dashboards.
- **Reporting is best-effort:** if the hub is off or busy, student boards give up
  fast (1.5 s timeouts) and keep working **standalone** — the puzzle, the LED and
  the kick-off all still function. **Pull the hub's power mid-game and every
  camper's board keeps running**; only the live scoreboard pauses.
- **Codenames:** keep them to letters, numbers and underscores. They're used as
  WiFi names and inside JSON, so avoid spaces and quotes.
- **Offline by design:** the dashboard uses no external libraries, no CDN and no
  browser storage — it works with no internet at all.

---

## 7. The "is this legal?" rule

The only "ban" in this game is a board shutting down **its own** network with
`WiFi.softAPdisconnect(true)`. That's it. There is **no** deauth, no attacking
*other* people's networks, and no jamming — that would be both illegal and against
the spirit of the camp. Cracking a puzzle just means reading the HTML a board
*chose* to send you. Keep it that way.
