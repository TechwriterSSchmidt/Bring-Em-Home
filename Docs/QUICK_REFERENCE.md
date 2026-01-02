# Quick Reference Guide - Bring Em Home

## 🚀 Quick Start (First Use)

1. **Power On**: Hold Button for **3 Seconds** (Vibration: Short-Short-Long).
2. **Wait for GPS**: Go outdoors and wait for satellite lock (1-5 min).
   - *Screen says: "Searching SATs"*
3. **Confirm Home**: When GPS is found, the screen asks "SET HOME?".
   - **1 Click**: Set **NEW** Home (Current Location).
   - **2 Clicks**: Load **SAVED** Home (e.g., Car).

## 🎯 Navigation Modes

### 1. Explore Mode (Default)
- **Display**: Shows Compass, Speed, and Distance from Home.
- **Buddy Tracking**: Shows a hollow arrow pointing to your partner if they are nearby.
- **Function**: Records "Breadcrumbs" (Waypoints) automatically as you walk.
- **Action**: Enjoy your hike!

### 2. Return Mode (Backtrack)
- **Activate**: Double-Click Button.
- **Display**: Shows Arrow pointing to the next Breadcrumb (or Home).
- **Function**: Guides you back along your exact path.
- **Feedback**: Vibration when reaching a waypoint.

## 📱 Display Guide (OLED)

| Icon/Text | Meaning |
|-----------|---------|
| **House Icon** | Your starting point (Home). |
| **Solid Arrow** | Direction to Home/Waypoint. Keep it pointing UP. |
| **Hollow Arrow** | Direction to your Buddy (Partner Device). |
| **B: 120m** | Distance to Buddy (Top Left). "?" means old data. |
| **Bars (Top R)** | GPS Signal Strength (1-4 bars). |
| **Bat %** | Battery Level. |
| **C:Good** | Compass Calibration Status. |

## 🔘 Button Functions

| Action | Gesture | Function |
|--------|---------|----------|
| **Toggle Display** | 1 Click | Screen ON/OFF (Saves Power) |
| **Switch Mode** | 2 Clicks | Explore ↔ Return Mode |
| **Flashlight** | 3 Clicks | Toggle LED Light |
| **SOS Mode** | 5 Clicks | Start SOS Countdown (LoRa Beacon) |
| **Panic Mode** | Hold 3-6s | **IMMEDIATE** Return Mode (Safety) |
| **Power OFF** | Hold >6s | Deep Sleep (Vibration: Long-Short-Short) |
| **Confirm Home** | 1/2 Clicks | 1x=New Home, 2x=Load Saved (At Boot) |

## ⚡ Charging & Battery

- **Charging**: Connect USB-C. External LED (WS2812) pulses Green.
- **Full**: LED solid Green.
- **Low Battery**: LED flashes Yellow (< 10%).

## 📊 Technical Info

- **MCU**: nRF52840 (Heltec T114)
- **LoRa**: SX1262 (868 MHz)
- **GPS**: L76K GNSS
- **Compass**: BNO055 (I2C 0x28)
- **Display**: SH1107 (I2C 0x3C)
