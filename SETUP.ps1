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

# Returns $true if a file exists AND is a real binary (not a Git LFS pointer stub)
function Test-RealFile($path) {
    if (-not (Test-Path $path)) { return $false }
    $bytes = [System.IO.File]::ReadAllBytes($path)
    if ($bytes.Length -lt 100) { return $false }   # LFS pointer is ~130 bytes of text
    # LFS pointers start with "version https://git-lfs"
    $header = [System.Text.Encoding]::ASCII.GetString($bytes[0..22])
    if ($header -like "version https://git-lfs*") { return $false }
    return $true
}

function Get-FileIfMissing($path, $url, $label) {
    if (Test-RealFile $path) { return $true }
    Write-WARN "$label not found or is a Git LFS stub. Downloading from official source..."
    Write-WARN "URL: $url"
    try {
        $wc = New-Object System.Net.WebClient
        $wc.DownloadFile($url, $path)
        Write-OK "Downloaded $label."
        return $true
    } catch {
        Write-FAIL "Download failed: $_"
        return $false
    }
}

Write-Host ""
Write-Host "============================================" -ForegroundColor Green
Write-Host "  ESP32 CYBER ACADEMY - SETUP SCRIPT" -ForegroundColor Green
Write-Host "  URJ Sci-Tech 6P" -ForegroundColor Green
Write-Host "============================================" -ForegroundColor Green

# -- Step 1: Install Arduino IDE ------------------------------
Write-Step 1 "Installing Arduino IDE 2.x..."
$arduinoExe = "$installersDir\arduino-ide-windows.exe"
$arduinoUrl = "https://downloads.arduino.cc/arduino-ide/arduino-ide_2.3.6_Windows_64bit.exe"

if (Get-FileIfMissing $arduinoExe $arduinoUrl "Arduino IDE installer") {
    Write-OK "Running silent install (takes ~2 minutes)..."
    $proc = Start-Process -FilePath $arduinoExe -ArgumentList "/S" -Wait -PassThru
    if ($proc.ExitCode -eq 0) {
        Write-OK "Arduino IDE installed."
    } else {
        Write-WARN "Installer exited with code $($proc.ExitCode). Arduino IDE may still have installed correctly."
    }
} else {
    Write-FAIL "Could not install Arduino IDE. Download manually from: https://www.arduino.cc/en/software"
}

# -- Step 2: Install CH340 USB driver -------------------------
Write-Step 2 "Installing CH340 USB driver..."
$ch340 = "$installersDir\CH341SER.EXE"
$ch340Url = "https://www.wch-ic.com/downloads/file/65.html"

if (Test-RealFile $ch340) {
    $proc = Start-Process -FilePath $ch340 -ArgumentList "/S" -Wait -PassThru
    Write-OK "CH340 driver installed."
} else {
    Write-WARN "CH341SER.EXE not found. Download and install manually:"
    Write-WARN "  https://www.wch-ic.com/downloads/CH341SER_EXE.html"
}

# -- Step 3: Install CP2102 USB driver ------------------------
Write-Step 3 "Installing CP2102 USB driver..."
$cp2102zip = "$installersDir\CP210x_Windows_Drivers.zip"
$cp2102dir = "$installersDir\cp2102-temp"
$cp2102Url = "https://www.silabs.com/documents/public/software/CP210x_Windows_Drivers.zip"

if (Get-FileIfMissing $cp2102zip $cp2102Url "CP2102 driver ZIP") {
    Write-OK "Extracting CP2102 drivers..."
    try {
        Expand-Archive -Path $cp2102zip -DestinationPath $cp2102dir -Force
        $cp2102Installer = Get-ChildItem "$cp2102dir" -Filter "CP210xVCPInstaller_x64.exe" -Recurse | Select-Object -First 1
        if ($cp2102Installer) {
            Start-Process -FilePath $cp2102Installer.FullName -ArgumentList "/S" -Wait
            Write-OK "CP2102 driver installed."
        } else {
            Write-WARN "Could not find CP210xVCPInstaller_x64.exe inside the zip."
        }
    } catch {
        Write-FAIL "Failed to extract ZIP: $_"
    }
} else {
    Write-FAIL "Could not install CP2102 driver. Download manually from: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers"
}

# -- Step 4: Install ESP32 board package via arduino-cli ------
Write-Step 4 "Installing ESP32 board package for Arduino..."
$cli = "$installersDir\arduino-cli.exe"
$cliUrl = "https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Windows_64bit.zip"
$cliZip = "$installersDir\arduino-cli.zip"

if (-not (Test-RealFile $cli)) {
    Write-WARN "arduino-cli.exe not found or is a Git LFS stub. Downloading..."
    if (Get-FileIfMissing $cliZip $cliUrl "arduino-cli ZIP") {
        $cliTemp = "$installersDir\cli-temp"
        Expand-Archive -Path $cliZip -DestinationPath $cliTemp -Force
        $cliExtracted = Get-ChildItem $cliTemp -Filter "arduino-cli.exe" -Recurse | Select-Object -First 1
        if ($cliExtracted) {
            Copy-Item $cliExtracted.FullName $cli -Force
            Write-OK "arduino-cli ready."
        } else {
            Write-FAIL "Could not find arduino-cli.exe in downloaded ZIP."
        }
    }
}

if (Test-RealFile $cli) {
    Write-OK "arduino-cli found. Configuring ESP32 boards (needs internet, ~5-10 min)..."
    & $cli config init --overwrite 2>&1 | Out-Null
    & $cli config add board_manager.additional_urls "https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json" 2>&1 | Out-Null
    Write-OK "Updating board index..."
    & $cli core update-index 2>&1 | Out-Null
    Write-OK "Installing esp32 core (downloading ~400MB - please wait)..."
    & $cli core install esp32:esp32 2>&1
    if ($LASTEXITCODE -eq 0) {
        Write-OK "ESP32 board package installed!"
    } else {
        Write-WARN "esp32 core install may have had issues (exit code $LASTEXITCODE). Check output above."
    }
} else {
    Write-FAIL "arduino-cli not available. Add ESP32 boards manually in Arduino IDE:"
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
