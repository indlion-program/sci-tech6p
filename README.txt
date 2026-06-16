============================================================
  ESP32 CYBER ACADEMY — SCI-TECH 6P CAMPER FOLDER
  URJ Sci-Tech Summer Camp
============================================================

HOW TO SET UP A CAMPER COMPUTER
--------------------------------
1. Copy this entire "sci-tech6p" folder to:
     C:\Users\<camper>\Documents\students\sci-tech6p\

2. Double-click SETUP.bat (runs as Admin)
   This will:
   - Install Arduino IDE 2.x
   - Install CH340 USB driver
   - Install CP2102 USB driver
   - Install the ESP32 board package
   - Copy code shortcuts to the Desktop

3. Plug in the ESP32 and confirm a COM port appears in Arduino IDE:
     Tools → Port
   If no port appears → the USB cable is data-only (charge-only won't work).

WHAT'S IN THIS FOLDER
-----------------------
installers\
   arduino-ide-windows.exe   Arduino IDE 2.x installer
   CH341SER.EXE              CH340/CH341 USB driver (most ESP32 boards)
   CP210x_Windows_Drivers.zip CP2102 USB driver (some boards)
   arduino-cli.exe           Arduino CLI — installs ESP32 board package

code\
   day1_hello_world\         Day 1 complete reference code
   day2_encryption\          Day 2 complete reference code
   day3_hidden_flag\         Day 3 complete reference code
   day4_ctf_puzzle\          Day 4 complete reference code (with LED)

explorer\
   day1_explorer\ … day4_explorer\   Fill-in-the-blank templates (ages 10-12)

expert\
   day1_expert\ … day4_expert\   Skeletal templates (ages 13-16)

tools\
   base64_encoder.html       Offline Base64 encode/decode tool
                             Open in Chrome — works with no internet!

ARDUINO IDE BOARD SETTINGS
----------------------------
Board:   Tools → Board → ESP32 Arduino → ESP32 Dev Module
Upload Speed: 115200
Port:    Tools → Port → COMx (changes each time you plug in)

If the upload fails:
  - Close Serial Monitor before uploading
  - Hold the BOOT button on the ESP32 while clicking Upload
  - Try a different USB cable (data-capable, not charge-only)

DAILY CODE GUIDE
-----------------
Day 1 (The Invisible Network):
  → Open: code\day1_hello_world\day1_hello_world.ino
  → Change the ssid to the camper's name
  → Flash and visit 192.168.4.1 on any device

Day 2 (The Art of Scrambling):
  → Open: tools\base64_encoder.html in Chrome
  → Encode a message, paste into code\day2_encryption\day2_encryption.ino

Day 3 (Behind the Curtain):
  → Open: code\day3_hidden_flag\day3_hidden_flag.ino
  → Change the FLAG comment to WORD-WORD-NUMBER format

Day 4 (Cyber Escape Room):
  → Open: code\day4_ctf_puzzle\day4_ctf_puzzle.ino
  → LED wiring: GPIO 2 → 330Ω resistor → LED → GND

TROUBLESHOOTING
----------------
Board doesn't show in Tools → Port:
  USB cable is charge-only. Swap to a data cable.
  Reinstall CH340 driver (installers\CH341SER.EXE).

Upload error "Failed to connect to ESP32":
  Hold the BOOT button on the ESP32 board while clicking Upload.
  Release after the "Connecting..." message appears.

Base64 encoder doesn't work:
  Make sure you're opening it in Chrome or Firefox (not Edge).
  It's a local HTML file — no internet required.

============================================================
