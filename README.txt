============================================================
  CAMP CYBER DIVISION - SCI-TECH 6P CAMPER FOLDER
  URJ Sci-Tech Summer Camp  -  Explorer track
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
   - Copy the code to the Desktop (ESP32_Code\, ESP32_Tools\)

3. Plug in the ESP32 and confirm a COM port appears in Arduino IDE:
     Tools -> Port
   If no port appears -> the USB cable is charge-only (won't work).

WHAT'S IN THIS FOLDER
-----------------------
student_board\
   student_board.ino   The one sketch every camper uses. You only edit the
                       two zones at the top (codename, password, flag,
                       secret path, and the page you paste from W3Schools).

teacher_hub\
   teacher_hub.ino     Flash this onto ONE board - the teacher's live
                       dashboard. Join Wi-Fi "CyberDivision_Hub" and open
                       http://10.0.0.1/ on the projector laptop.

slides\
   camp_slides.html    The 4-day slide deck. Open in any browser.
                       Arrow keys or click to move between slides.

tools\
   base64_encoder.html Offline base64 encode/decode tool. Open in Chrome -
                       works with no internet.

THE 4-DAY PLAN (Explorer)
-------------------------
Day 1 - Get it running:  flash student_board, open 192.168.4.1, meet the
                         browser Network tab.
Day 2 - Make it yours:   paste a whole page from W3Schools into the PASTE
                         slot; flash and watch it change.
Day 3 - Secrets:         set your secretPath (the board broadcasts it in a
                         base64 "X-Access" header); hide your flag in the
                         page source; the secret page checks the flag.
Day 4 - Play:            attack each other's boards; the teacher_hub board
                         shows who's been breached, live.

THE WIN CHAIN
-------------
   join a board's Wi-Fi
   -> find the flag hidden in the page source
   -> Network tab: read the X-Access header (base64) -> decode -> a path
   -> open that path, submit the flag
   -> correct = YOU PWNED [name]  (and the dashboard marks them breached)
      wrong   = ACCESS DENIED

ARDUINO IDE BOARD SETTINGS
----------------------------
Board:   Tools -> Board -> ESP32 Arduino -> ESP32 Dev Module
Upload Speed: 115200
Port:    Tools -> Port -> COMx (changes each time you plug in)

If the upload fails:
  - Close Serial Monitor before uploading
  - Hold the BOOT button on the ESP32 while clicking Upload
  - Try a different USB cable (data-capable, not charge-only)

TROUBLESHOOTING
----------------
Board doesn't show in Tools -> Port:
  USB cable is charge-only. Swap to a data cable.
  Reinstall CH340 driver (installers\CH341SER.EXE).

Upload error "Failed to connect to ESP32":
  Hold the BOOT button on the ESP32 board while clicking Upload.
  Release after the "Connecting..." message appears.

Base64 tool doesn't work:
  Open it in Chrome or Firefox. It's a local HTML file - no internet needed.

============================================================
