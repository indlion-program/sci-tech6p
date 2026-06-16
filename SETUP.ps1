# ============================================================
# ESP32 Cyber Academy - Computer Setup Script
# URJ Sci-Tech Summer Camp
#
# Run this script on EACH camper computer before Day 1.
# Right-click this file -> "Run with PowerShell" (as Admin)
# ============================================================

$ErrorActionPreference = "Continue"
$installersDir = "$PSScriptRoot\installers"

function Write-Step($n, $msg) {
    Write-Host ""
    Write-Host "[$n] $msg" -ForegroundColor Cyan
}

function Write-OK($msg)   { Write-Host "    OK: $msg" -ForegroundColor Green }
function Write-WARN($msg) { Write-Host "    !! $msg" -ForegroundColor Yellow }
function Write-FAIL($msg) { Write-Host "    FAIL: $msg" -ForegroundColor Red }

Write-Host ""
Write-Host "============================================" -ForegroundColor Green
Write-Host "  ESP32 CYBER ACADEMY - SETUP SCRIPT" -ForegroundColor Green
Write-Host "  URJ Sci-Tech 6P" -ForegroundColor Green
Write-Host "============================================" -ForegroundColor Green

# -- Step 1: Install Arduino IDE ------------------------------
Write-Step 1 "Installing Arduino IDE 2.x..."
$arduinoExe = "$installersDir\arduino-ide-windows.exe"
if (Test-Path $arduinoExe) {
    Write-OK "Installer found. Running silent install (takes ~2 minutes)..."
    Start-Process -FilePath $arduinoExe -ArgumentList "/S" -Wait
    Write-OK "Arduino IDE installed."
} else {
    Write-WARN "arduino-ide-windows.exe not found in installers\ folder."
    Write-WARN "Download from: https://www.arduino.cc/en/software"
    Write-WARN "Install manually, then re-run this script."
}

# -- Step 2: Install CH340 USB driver -------------------------
Write-Step 2 "Installing CH340 USB driver..."
$ch340 = "$installersDir\CH341SER.EXE"
if (Test-Path $ch340) {
    Start-Process -FilePath $ch340 -ArgumentList "/S" -Wait
    Write-OK "CH340 driver installed."
} else {
    Write-WARN "CH341SER.EXE not found in installers\ folder."
    Write-WARN "Download from: https://www.wch-ic.com/downloads/CH341SER_EXE.html"
    Write-WARN "Install manually."
}

# -- Step 3: Install CP2102 USB driver ------------------------
Write-Step 3 "Installing CP2102 USB driver..."
$cp2102zip = "$installersDir\CP210x_Windows_Drivers.zip"
$cp2102dir = "$installersDir\cp2102-temp"
if (Test-Path $cp2102zip) {
    Write-OK "Extracting CP2102 drivers..."
    Expand-Archive -Path $cp2102zip -DestinationPath $cp2102dir -Force
    $cp2102Installer = Get-ChildItem "$cp2102dir" -Filter "CP210xVCPInstaller_x64.exe" -Recurse | Select-Object -First 1
    if ($cp2102Installer) {
        Start-Process -FilePath $cp2102Installer.FullName -ArgumentList "/S" -Wait
        Write-OK "CP2102 driver installed."
    } else {
        Write-WARN "Could not find CP210xVCPInstaller_x64.exe inside the zip."
    }
} else {
    Write-WARN "CP210x_Windows_Drivers.zip not found in installers\ folder."
    Write-WARN "Download from: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers"
}

# -- Step 4: Install ESP32 board package via arduino-cli ------
Write-Step 4 "Installing ESP32 board package for Arduino..."
$cli = "$installersDir\arduino-cli.exe"
if (Test-Path $cli) {
    Write-OK "arduino-cli found. Configuring ESP32 boards (needs internet, ~5-10 min)..."
    & $cli config init --overwrite 2>&1 | Out-Null
    & $cli config add board_manager.additional_urls "https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json" 2>&1 | Out-Null
    Write-OK "Updating board index..."
    & $cli core update-index 2>&1 | Out-Null
    Write-OK "Installing esp32 core (downloading ~400MB - please wait)..."
    & $cli core install esp32:esp32 2>&1
    Write-OK "ESP32 board package installed!"
} else {
    Write-WARN "arduino-cli.exe not found in installers\ folder."
    Write-WARN "Manually add board URL in Arduino IDE:"
    Write-WARN "  File > Preferences > Additional Boards Manager URLs:"
    Write-WARN "  https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json"
    Write-WARN "Then: Tools > Board > Boards Manager > search 'esp32' > Install by Espressif Systems"
}

# -- Step 5: Copy code to Desktop for easy access -------------
Write-Step 5 "Copying code folder to Desktop..."
$desktop = [Environment]::GetFolderPath("Desktop")
Copy-Item -Path "$PSScriptRoot\code" -Destination "$desktop\ESP32_Code" -Recurse -Force
Copy-Item -Path "$PSScriptRoot\explorer" -Destination "$desktop\ESP32_Code\explorer" -Recurse -Force
Copy-Item -Path "$PSScriptRoot\expert" -Destination "$desktop\ESP32_Code\expert" -Recurse -Force
Copy-Item -Path "$PSScriptRoot\tools" -Destination "$desktop\ESP32_Tools" -Recurse -Force
Write-OK "Code is on the Desktop in ESP32_Code\ and ESP32_Tools\"

# -- Done -----------------------------------------------------
Write-Host ""
Write-Host "============================================" -ForegroundColor Green
Write-Host "  SETUP COMPLETE!" -ForegroundColor Green
Write-Host "============================================" -ForegroundColor Green
Write-Host ""
Write-Host "Next steps:" -ForegroundColor White
Write-Host "  1. Open Arduino IDE" -ForegroundColor White
Write-Host "  2. Plug in an ESP32 board via USB" -ForegroundColor White
Write-Host "  3. Tools > Board > ESP32 Arduino > ESP32 Dev Module" -ForegroundColor White
Write-Host "  4. Tools > Port > select the COM port that appears" -ForegroundColor White
Write-Host "  5. Open Desktop\ESP32_Code\day1_hello_world\day1_hello_world.ino" -ForegroundColor White
Write-Host "  6. Click Upload (arrow button). If it works, you're ready!" -ForegroundColor White
Write-Host ""
Write-Host "If no COM port appears after plugging in the ESP32:" -ForegroundColor Yellow
Write-Host "  -> The USB cable is charge-only. Swap it for a data cable." -ForegroundColor Yellow
Write-Host "  -> Or reinstall the CH340 driver manually." -ForegroundColor Yellow
Write-Host ""
Read-Host "Press Enter to close"
