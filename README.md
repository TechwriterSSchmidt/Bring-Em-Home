# Bring Em Home 🧭

A GPS navigation device to guide Emilie back to her starting location when lost on hikes.

## Overview

This device uses GPS and compass sensors to help navigate back to a saved "home" position. Perfect for hikers who want peace of mind knowing they can always find their way back to their starting point.

## Hardware Requirements

- **ESP32-S3 Development Board** with [Waveshare ESP32-S3-LCD-1.3 (240x240)](https://www.waveshare.com/esp32-s3-lcd-1.28.htm) (Note: Link is for similar family, specific 1.3" square model used)
- **HGLRC M100-5883 M10 GPS Module** with integrated HMC5883L compass
- Connecting wires
- USB cable for programming and power

## Hardware Connections

### GPS Module (UART Connection)
- GPS TX → ESP32-S3 GPIO 18 (RX)
- GPS RX → ESP32-S3 GPIO 17 (TX)
- GPS VCC → 5V
- GPS GND → GND

### Compass/Magnetometer (I2C Connection)
The HMC5883L compass is integrated in the GPS module:
- SDA → ESP32-S3 GPIO 8
- SCL → ESP32-S3 GPIO 9
- VCC → 3.3V
- GND → GND

### Display
The Waveshare ESP32-S3-LCD-1.3 display is built into the board:
- Display Driver: ST7789
- Resolution: 240x240
- Pre-wired to the following pins:
  - CS: GPIO 39
  - DC: GPIO 38
  - RST: GPIO 42
  - MOSI: GPIO 41
  - SCLK: GPIO 40
  - Backlight: GPIO 45

### Buttons
- **External Waterproof Button (GPIO 14)**:
  - **Short Press**: Toggle Display ON / OFF (Timer resets to 5 min when turned ON)
  - **Long Press (> 2s)**: Save current position as home
- **BOOT Button (GPIO 0)**: Not used in housing

## Software Setup

### Prerequisites
- [PlatformIO](https://platformio.org/) (recommended) or Arduino IDE
- USB drivers for ESP32-S3

### Building with PlatformIO

1. Clone this repository:
   ```bash
   git clone https://github.com/TechwriterSSchmidt/Bring_Em_Home.git
   cd Bring_Em_Home
   ```

2. Build the project:
   ```bash
   pio run
   ```

3. Upload to the ESP32-S3:
   ```bash
   pio run --target upload
   ```

4. Monitor serial output:
   ```bash
   pio device monitor
   ```

### Building with Arduino IDE

1. Install the following libraries via Library Manager:
   - TinyGPSPlus
   - GFX Library for Arduino (MoonOnOurNation)
   - MechaQMC5883 (Mechasolution)

2. Select Board: "ESP32S3 Dev Module"
3. Set partition scheme to default
4. Open `src/main.cpp` and upload

## Usage

### First Time Setup

1. Power on the device
2. Wait for GPS to acquire satellite lock (may take 1-5 minutes outdoors with clear sky view)
3. Once GPS shows valid coordinates, **press and hold the external button for 2 seconds** to save your current position as "home"
4. The screen will flash green and show "HOME SAVED!"

### Navigation

- The screen displays:
  - **Distance** to home (in meters or km)
  - **Direction Arrow** pointing towards home relative to your current heading
  - **Breadcrumb Count**: Number of auto-saved waypoints (saved every 2.5 minutes)
- Follow the arrow to return to your starting point.

### Navigation Modes

The device has two modes, toggled by a **Double Click** on the button:

1.  **Recording Mode (Default)**
    *   **Green Arrow**: Points to **North** (Compass).
    *   **Display**: Shows distance to Home.
    *   **Action**: Automatically saves breadcrumbs every 250m.

2.  **Backtracking Mode (Return)**
    *   **Red Arrow**: Points to the **Next Waypoint** or **Home**.
    *   **Display**: Shows distance to the target.
    *   **Action**: Guides you back along your path.

### Power Saving

- The display automatically turns off after **5 minutes** of inactivity.
- **Single Click** the button to toggle the display ON or OFF.
- Turning the display ON resets the 5-minute timer.

### Navigation Indicators

The display shows:
- **GPS Status**: Satellite count
- **Coordinates**: Real-time Lat/Lon at the bottom
- **Compass Heading**: Current direction (0-360°)
- **Distance**: To Home or Next Waypoint
- **Visual Arrow**: 
    - **Green (Recording)**: North Indicator
    - **Red (Backtracking)**: Direction to Target

### Finding Your Way Home

1. **Double Click** the button to enter **Backtracking Mode** (Screen turns Blue briefly).
2. Follow the **Red Arrow**.
3. The device will guide you from waypoint to waypoint until you reach Home.

## Features

- ✅ Real-time GPS coordinate display
- ✅ Save home position with button press (Long Press)
- ✅ Auto-save Home position after 5 minutes if not set
- ✅ Breadcrumb trail (auto-save every 250m)
- ✅ Backtracking mode to retrace steps
- ✅ Calculate distance to home/waypoint
- ✅ Digital compass (Green North Arrow)
- ✅ Visual navigation arrow (Red Target Arrow)
- ✅ Persistent storage of home position
- ✅ Satellite count display
- ✅ Low power consumption (Auto-off)

## Display Color Coding

- 🟡 **Yellow**: Title and home status
- 🟢 **Green**: Active GPS lock and navigation info
- 🔴 **Red**: GPS searching/no lock
- 🔵 **Cyan**: Coordinates
- 🟣 **Magenta**: Compass heading
- 🟠 **Orange**: Warnings/no home set

## Troubleshooting

### GPS Not Locking
- Ensure you're outdoors with clear view of sky
- GPS may take 1-5 minutes for initial lock
- Avoid urban canyons, dense forests, or indoor use
- Check GPS module connections

### Compass Reading Incorrect
- Keep device away from magnetic materials (metal, speakers, motors)
- Calibrate by moving device in figure-8 pattern
- Check I2C connections to HMC5883L

### Display Not Working
- Check power supply (USB should provide 5V)
- Verify backlight is enabled
- Check SPI connections

### Home Position Not Saving
- Ensure GPS has valid lock before saving
- Check that flash memory is not full
- Try power cycling the device

## Technical Details

### GPS Specifications
- Update rate: 1-10 Hz
- Accuracy: ~2.5m CEP
- Cold start time: ~30s
- Warm start time: ~2s

### Compass Specifications
- Resolution: 0.73 mGauss
- Range: ±8 Gauss
- Accuracy: 1-2° heading accuracy

### Power Consumption
- Active (GPS + Display): ~150-200mA @ 5V
- Sleep mode: Not yet implemented

## Future Enhancements

- [ ] Add waypoint system (multiple saved locations)
- [ ] Track and display hiking path
- [ ] Battery percentage display
- [ ] Power saving sleep mode
- [ ] Touch screen interface for settings
- [ ] Log coordinates to SD card
- [ ] Add altitude display
- [ ] Speed and time estimates
- [ ] Breadcrumb trail feature

## Contributing

Feel free to open issues or submit pull requests for improvements!

## License

This project is open source. Feel free to use and modify as needed.

## Credits

Created to help Emilie find her way home on hikes! 🏔️➡️🏠
