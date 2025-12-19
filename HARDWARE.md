# Hardware Setup Guide

## Components List

### Main Components
1. **Waveshare ESP32-S3 Touch LCD 1.47"**
   - ESP32-S3 dual-core processor
   - 172x320 pixel ST7789V LCD display
   - Built-in touchscreen
   - USB-C connector
   - [Product Link](https://www.waveshare.com/esp32-s3-touch-lcd-1.47.htm)

2. **HGLRC M100-5883 M10 GPS Module**
   - u-blox M10 GPS receiver
   - Integrated HMC5883L magnetometer/compass
   - UART interface for GPS
   - I2C interface for compass
   - External antenna support

### Additional Materials
- Jumper wires (female-to-female recommended)
- USB-C cable for programming and power
- Optional: Battery pack for portable use
- Optional: 3D printed enclosure

## Pinout Diagram

### ESP32-S3 to GPS Module

```
ESP32-S3              GPS Module (M100-5883)
========              ======================
GPIO 18 (RX1) <-----> TX (GPS UART)
GPIO 17 (TX1) <-----> RX (GPS UART)
5V            ------> VCC
GND           ------> GND
```

### ESP32-S3 to Compass (I2C on GPS Module)

```
ESP32-S3              HMC5883L (on GPS Module)
========              ========================
GPIO 8 (SDA)  <-----> SDA
GPIO 9 (SCL)  <-----> SCL
3.3V          ------> VCC
GND           ------> GND
```

### Display (Built-in on Waveshare Board)

The display is pre-wired on the Waveshare ESP32-S3 Touch LCD board:

```
Display Pin    ESP32-S3 GPIO
===========    =============
CS      -----> GPIO 10
DC      -----> GPIO 13
RST     -----> GPIO 14
MOSI    -----> GPIO 11
SCLK    -----> GPIO 12
BLK     -----> GPIO 38
```

## Assembly Instructions

### Step 1: GPS Module Connection

1. Identify the GPS module's UART pins:
   - TX (transmit from GPS)
   - RX (receive to GPS)
   - VCC (5V power)
   - GND (ground)

2. Connect to ESP32-S3:
   ```
   GPS TX  → ESP32 GPIO 18
   GPS RX  → ESP32 GPIO 17
   GPS VCC → ESP32 5V
   GPS GND → ESP32 GND
   ```

### Step 2: Compass/Magnetometer Connection

The HMC5883L compass is integrated on the GPS module board:

1. Locate the I2C pins (usually separate from GPS UART):
   - SDA (data line)
   - SCL (clock line)
   - VCC (3.3V recommended)
   - GND (ground)

2. Connect to ESP32-S3:
   ```
   Compass SDA → ESP32 GPIO 8
   Compass SCL → ESP32 GPIO 9
   Compass VCC → ESP32 3.3V
   Compass GND → ESP32 GND
   ```

### Step 3: Power Connection

- Connect USB-C cable to ESP32-S3 board
- This provides both programming interface and power
- For portable use, consider a USB power bank

### Step 4: Physical Assembly

1. **Temporary Setup** (for testing):
   - Use breadboard and jumper wires
   - Keep GPS module elevated for better satellite reception

2. **Permanent Setup**:
   - Consider using a small project box or 3D printed enclosure
   - GPS antenna should be positioned on top for best reception
   - Keep compass away from magnetic interference
   - Mount display facing outward
   - Add strain relief for wires

## Testing Hardware Connections

### Initial Power-On Test

1. Connect USB cable
2. Display should light up and show boot screen
3. Check serial monitor at 115200 baud

### GPS Test

Expected serial output:
```
Bring Em Home - Starting...
GPS initialized
GPS: SEARCHING...
```

After 1-5 minutes outdoors:
```
GPS: LOCKED
GPS: 48.123456, 11.123456 | Heading: 45.0° | No home set
```

### Compass Test

Expected serial output during initialization:
```
HMC5883L detected!
```

If you see:
```
No HMC5883L detected
```

Check I2C connections and I2C address (default is 0x1E).

### Display Test

Display should show:
- Title "Bring Em Home" in yellow
- GPS status (red if searching, green if locked)
- Current coordinates
- Compass heading
- Instructions

## Troubleshooting Hardware

### GPS Not Working

**Symptoms**: "GPS: SEARCHING..." never changes

**Solutions**:
1. Verify GPS module has power (LED may blink)
2. Check UART wiring (TX/RX not swapped)
3. Move outdoors with clear sky view
4. Check baud rate (should be 9600)
5. Try swapping TX/RX if still not working

### Compass Not Working

**Symptoms**: "Compass: FAIL" on boot screen

**Solutions**:
1. Check I2C wiring
2. Verify I2C address with scanner sketch
3. Ensure 3.3V power (not 5V which may damage sensor)
4. Check for I2C pull-up resistors (usually built-in)

### Display Issues

**Symptoms**: Black screen or garbled display

**Solutions**:
1. Check backlight pin (GPIO 38)
2. Verify SPI connections
3. Check power supply (needs stable 5V)
4. Adjust display rotation in code if needed

### Magnetic Interference

**Symptoms**: Compass readings jump around or incorrect

**Solutions**:
1. Keep away from metal, motors, speakers
2. Don't mount near large batteries
3. Calibrate by moving in figure-8 pattern
4. Ensure wires are not creating magnetic loops

## Power Consumption

Typical current draw:
- Display on: ~120mA
- GPS active: ~40mA
- ESP32-S3: ~30mA
- Total: ~190mA @ 5V (~1W)

For portable use:
- 10,000mAh power bank: ~50 hours runtime
- 5,000mAh power bank: ~25 hours runtime

## Enclosure Design Considerations

When designing or selecting an enclosure:

1. **GPS Antenna**: Must have clear view of sky
   - Mount on top of enclosure
   - Use plastic/acrylic window above antenna
   - Avoid metal enclosures

2. **Display**: Accessible and visible
   - Front-facing mount
   - Consider anti-glare protection
   - Touchscreen requires exposed glass

3. **Buttons**: Easy access
   - Boot button should be accessible
   - Consider external buttons via GPIO

4. **Ventilation**: ESP32-S3 generates heat
   - Small ventilation holes
   - Don't seal completely

5. **Mounting**: For hiking use
   - Belt clip or carabiner attachment
   - Wrist strap consideration
   - Shock absorption padding

## Safety Considerations

⚠️ **Important Safety Notes**:

1. **GPS is NOT a replacement for map and compass skills**
2. Always carry backup navigation tools
3. Device requires clear sky view - won't work indoors/underground
4. Battery can die - have backup power or route plan
5. Accuracy is ~2-5 meters, not precise enough for technical navigation
6. Compass can be affected by magnetic interference
7. This is a navigational aid, not a safety device

## Advanced Modifications

### Adding External GPS Antenna

Some M100 modules support external antenna:
1. Connect to u.FL connector on module
2. Mount antenna on top of enclosure
3. Improves signal in difficult conditions

### Adding Battery Monitoring

Add voltage divider to monitor battery:
```
Battery+ → Resistor 10kΩ → GPIO 4 → Resistor 10kΩ → GND
```

Then read ADC to estimate battery level.

### Weatherproofing

For outdoor durability:
1. Use IP67 rated enclosure
2. Seal all wire entry points
3. Add silica gel desiccant inside
4. Use waterproof connectors

## Bill of Materials (BOM)

| Item | Quantity | Est. Price |
|------|----------|------------|
| Waveshare ESP32-S3 Touch LCD 1.47" | 1 | $15-20 |
| HGLRC M100-5883 M10 GPS Module | 1 | $15-25 |
| Jumper Wires (F-F) | 8+ | $2-5 |
| USB-C Cable | 1 | $3-5 |
| Optional: Project Box | 1 | $5-10 |
| Optional: USB Power Bank | 1 | $10-20 |
| **Total** | | **~$50-85** |

## Recommended Tools

- Soldering iron (if soldering connections)
- Wire strippers
- Multimeter (for testing connections)
- Small screwdrivers
- Hot glue gun (for strain relief)

## Next Steps

After assembly:
1. Flash the firmware (see README.md)
2. Test indoors first (GPS won't lock, but system should boot)
3. Take outdoors for GPS lock test
4. Calibrate compass by moving in figure-8 pattern
5. Save home position and test navigation

For software setup and usage, see [README.md](README.md).
