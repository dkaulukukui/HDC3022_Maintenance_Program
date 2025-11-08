# HDC Sensor Maintenance Utility - User Guide

## Overview

This is a **standalone utility program** designed specifically for HDC302x sensor calibration and maintenance. Unlike the full environmental monitoring system, this lightweight program focuses exclusively on HDC sensor maintenance operations.

---

## What You Need

### Hardware Requirements:
1. **Adafruit Feather M0** (or compatible Arduino board)
2. **Single HDC302x sensor** (HDC3020/HDC3021/HDC3022)
3. **OLED Display** (SH1107, 128x64 pixels, I2C)
4. **Three buttons** (A, B, C)
5. **I2C pull-up resistors** (if not on breakout boards)

### Connections:

```
Feather M0          HDC302x Sensor
---------           --------------
3.3V    ----------> VIN
GND     ----------> GND
SDA     ----------> SDA
SCL     ----------> SCL

Feather M0          OLED Display
---------           -------------
3.3V    ----------> VIN
GND     ----------> GND  
SDA     ----------> SDA
SCL     ----------> SCL

Feather M0          Buttons
---------           -------
Pin 9   ----------> Button A (other side to GND)
Pin 6   ----------> Button B (other side to GND)
Pin 5   ----------> Button C (other side to GND)
```

**Note:** Buttons use internal pull-ups, so just connect one side to the pin and the other to GND.

---

## Features

### Main Menu Options:

1. **View Sensor Info**
   - Display NIST ID (serial number)
   - Current temperature and humidity
   - Current offset values
   - Real-time readings

2. **Condensation Removal**
   - Removes condensation from sensor element
   - Uses heater at half power
   - Runs until humidity < 1%
   - Timeout: 5 minutes

3. **Offset Error Correction**
   - Calibrates sensor for accuracy
   - Uses Look-Up Table algorithm
   - Writes calibration to sensor EEPROM
   - Timeout: 2 minutes

4. **Reset Offsets**
   - Clears calibration data
   - Writes 0.0 to both temp and humidity offsets
   - Immediate operation

---

## Button Controls

### In Main Menu:
- **Button A:** Navigate UP (previous menu item)
- **Button B:** SELECT / ENTER (choose current item)
- **Button C:** Navigate DOWN (next menu item)

### In Sensor Info:
- **Button C:** EXIT back to main menu

### In Confirmation Screens:
- **Button A:** CONFIRM (start operation)
- **Button C:** CANCEL (return to main menu)

### During Operations:
- All buttons disabled until operation completes
- Press any button to return after completion

---

## How to Use

### First Time Setup:

1. **Upload the program** to your Feather M0
2. **Open Serial Monitor** at 115200 baud (optional, for diagnostics)
3. **Watch the OLED** for initialization messages

**Expected startup:**
```
┌────────────────────┐
│ HDC Maintenance    │
│ Utility v1.0       │
│                    │
│ Initializing...    │
└────────────────────┘
        ↓
┌────────────────────┐
│ HDC302x Found!     │
│ Address: 0x44      │
│                    │
│ NIST ID:           │
│ 5449000030220001   │
│                    │
│ Ready!             │
└────────────────────┘
```

---

## Using the Main Menu

### Main Menu Display:
```
┌────────────────────┐
│ === MAIN MENU ===  │
│                    │
│ > 1.View Sensor    │
│   2.Condensation   │
│   3.Offset Corr.   │
│   4.Reset Offsets  │
│                    │
│ T:23.5C RH:55.2%   │ ← Live readings
└────────────────────┘
```

**Navigation:**
- Press **A** to move cursor up
- Press **C** to move cursor down
- Press **B** to select highlighted item

---

## 1. View Sensor Info

Shows detailed sensor information:

```
┌────────────────────┐
│ SENSOR INFO        │
│ ------------       │
│ ID:5449000030220001│ ← NIST ID
│ Temp: 23.5C        │ ← Current temp
│ RH:   55.2%        │ ← Current humidity
│ Offsets:           │
│  T:0.00 RH:-2.50   │ ← Current offsets
│                    │
│ Press C to exit    │
└────────────────────┘
```

**Updates in real-time** (every 500ms)

**To exit:** Press Button C

---

## 2. Condensation Removal

### When to Use:
- Sensor shows 99-100% humidity continuously
- After exposure to moisture
- Sensor readings seem stuck/unresponsive
- Routine maintenance in humid environments

### Confirmation Screen:
```
┌────────────────────┐
│ CONDENSATION       │
│ REMOVAL            │
│                    │
│ This will take     │
│ several minutes.   │
│                    │
│ A: Confirm         │
│ C: Cancel          │
└────────────────────┘
```

### During Operation:
```
┌────────────────────┐
│ CONDENSATION       │
│ ------------       │
│ T:45.3C (+21.8)    │ ← Temp & heat rise
│ RH:15.2%           │ ← Current humidity
│ Time:45s           │ ← Elapsed time
│                    │
│ Heating...         │ ← Status
└────────────────────┘
```

**Status Messages:**
- "Heating..." - Normal operation
- "Almost done!" - Humidity < 10%
- "Cooling..." - Heater off, cooldown period

### Success Screen:
```
┌────────────────────┐
│ CONDENSATION       │
│ REMOVAL            │
│                    │
│ SUCCESS!           │
│                    │
│ Final T:24.1C      │
│ Final RH:58.3%     │
│                    │
│ Press any button   │
└────────────────────┘
```

### Timeout Screen (if needed):
```
┌────────────────────┐
│ CONDENSATION       │
│ REMOVAL            │
│                    │
│ TIMEOUT            │
│                    │
│ Humidity did not   │
│ reach < 1%         │
│                    │
│ Press any button   │
└────────────────────┘
```

**Duration:** 30 seconds to 5 minutes (depends on initial humidity)

---

## 3. Offset Error Correction

### When to Use:
- Annual calibration (recommended)
- Sensor drift suspected
- After heavy use or contamination
- Regulatory compliance requirements

### Confirmation Screen:
```
┌────────────────────┐
│ OFFSET ERROR       │
│ CORRECTION         │
│                    │
│ This will take     │
│ several minutes.   │
│                    │
│ A: Confirm         │
│ C: Cancel          │
└────────────────────┘
```

### During Operation:
```
┌────────────────────┐
│ OFFSET CORR.       │
│ ------------       │
│ T:68.2C (+44.7)    │ ← Temp & heat rise
│ RH:8.5%            │ ← Current humidity
│ Time:65s           │ ← Elapsed time
│                    │
│ Target:+48C        │ ← Target from LUT
└────────────────────┘
```

**Status Messages:**
- "Target:+XXC" - Shows required temperature rise
- "Cooling..." - Heater off, cooldown period

### Success Screen:
```
┌────────────────────┐
│ OFFSET             │
│ CORRECTION         │
│                    │
│ SUCCESS!           │
│                    │
│ Temp:0.00C         │
│ RH:-2.53%          │
│                    │
│ Offsets saved      │
│ to sensor EEPROM   │
│                    │
│ Press any button   │
└────────────────────┘
```

**Duration:** 1-2 minutes typically

**Important:** Offsets are permanently saved to the sensor's EEPROM. They persist even after power cycling.

---

## 4. Reset Offsets

### When to Use:
- Starting fresh calibration
- Sensor replaced
- Testing/troubleshooting
- Clearing bad calibration

### Confirmation Screen:
```
┌────────────────────┐
│ RESET OFFSETS      │
│ TO ZERO            │
│                    │
│ This will take     │
│ several minutes.   │
│                    │
│ A: Confirm         │
│ C: Cancel          │
└────────────────────┘
```

### During Operation:
```
┌────────────────────┐
│ RESET OFFSETS      │
│                    │
│ Writing 0.0 to     │
│ sensor EEPROM...   │
└────────────────────┘
```

### Success Screen:
```
┌────────────────────┐
│ RESET OFFSETS      │
│                    │
│ SUCCESS!           │
│                    │
│ Temp:0.00C         │
│ RH:0.00%           │
│                    │
│ Press any button   │
└────────────────────┘
```

**Duration:** < 1 second

---

## Serial Monitor Output

The Serial Monitor (115200 baud) provides detailed diagnostic information:

### Startup:
```
=== HDC Sensor Maintenance Utility ===
Version 1.0
HDC found at 0x44
Using hardware serial number
Current offsets - Temp: 0.00°C, RH: -2.50%
Sensor initialized successfully!
I2C Address: 0x44
NIST ID: 5449000030220001
```

### During Condensation Removal:
```
=== Starting Condensation Removal ===
Initial Temp: 23.5°C, Initial RH: 65.2%
Heater enabled at HALF power
Temp: 28.3°C (+4.8°C), RH: 45.1%, Time: 5s
Temp: 35.2°C (+11.7°C), RH: 25.3%, Time: 10s
Temp: 42.1°C (+18.6°C), RH: 12.4%, Time: 15s
Temp: 48.5°C (+25.0°C), RH: 0.8%, Time: 20s
Condensation removed!
Heater disabled
Cooling down...
Final Temp: 24.1°C, Final RH: 58.3%
Condensation removal completed successfully
```

### During Offset Correction:
```
=== Starting Offset Error Correction ===
Initial Temp: 23.5°C, Initial RH: 55.2%
Target temperature rise: 48.01°C
Heater enabled at FULL power
Temp: 35.2°C, Rise: 11.7°C, RH: 32.4%
Temp: 48.3°C, Rise: 24.8°C, RH: 18.2%
Temp: 71.5°C, Rise: 48.0°C, RH: 3.2%
Target temperature reached
Heater disabled
Calculated humidity offset: 3.2% RH
Offsets written to sensor
Verified offsets - Temp: 0.00°C, RH: -3.20%
Cooling down...
Corrected readings - Temp: 23.7°C, RH: 55.1%
Offset correction completed successfully
```

---

## Troubleshooting

### "No HDC sensor detected!"

**Display shows:**
```
┌────────────────────┐
│ ERROR:             │
│                    │
│ No HDC sensor      │
│ detected!          │
│                    │
│ Check:             │
│ - I2C wiring       │
│ - Power supply     │
└────────────────────┘
```

**Solutions:**
1. Check I2C connections (SDA, SCL, VCC, GND)
2. Verify sensor has power (3.3V)
3. Check I2C pull-up resistors (required)
4. Try different I2C address (sensor might be 0x45 instead of 0x44)
5. Test with I2C scanner
6. Verify sensor is HDC302x (not HDC1080 or other variant)

---

### Condensation Removal Times Out

**Symptoms:** Shows "TIMEOUT" after 5 minutes

**Possible Causes:**
- Initial humidity very high (>95%)
- Ambient humidity very high
- Insufficient heater power
- Sensor contaminated

**Solutions:**
1. Run condensation removal again (may take multiple cycles)
2. Improve ventilation around sensor
3. Move to drier environment if possible
4. If persistent, sensor may need cleaning or replacement

---

### Offset Correction Fails

**Symptoms:** Shows "FAILED"

**Possible Causes:**
- Unstable environmental conditions
- Temperature or humidity out of range
- I2C communication error
- EEPROM write failure

**Solutions:**
1. Ensure stable environment (avoid drafts, temperature changes)
2. Verify temperature is 15-30°C and humidity 10-45%
3. Try again after waiting 30 seconds
4. Check I2C connections
5. Verify sensor is functioning (check View Sensor Info)

---

### NIST ID Shows Placeholder

**NIST ID like:** `DC30220000000001`

**This is normal if:**
- Serial number register not accessible
- Using older firmware
- HDC variant doesn't support serial number

**Not a problem:** System works perfectly with placeholder IDs. Real serial numbers are only needed for advanced traceability.

---

### Buttons Not Responding

**Solutions:**
1. Check button connections
2. Verify buttons connected to correct pins (9, 6, 5)
3. Ensure buttons ground the pin when pressed
4. Try pressing harder or longer
5. Check for shorts in wiring

---

## Technical Specifications

### Condensation Removal:
- **Heater Power:** 50% (Half power)
- **Success Criteria:** Humidity < 1%
- **Monitoring Interval:** 5 seconds
- **Timeout:** 300 seconds (5 minutes)
- **Cooldown:** 10 seconds
- **Typical Duration:** 30-120 seconds

### Offset Error Correction:
- **Heater Power:** 100% (Full power)
- **Algorithm:** HDC302x LUT-based (Section 3.7)
- **Monitoring Interval:** 2 seconds
- **Timeout:** 120 seconds (2 minutes)
- **Cooldown:** 10 seconds
- **Typical Duration:** 60-90 seconds
- **Persistence:** Written to sensor EEPROM

### Reset Offsets:
- **Operation:** Write 0.0 to temp and RH offsets
- **Persistence:** Written to sensor EEPROM
- **Duration:** <1 second
- **Verification:** Reads back to confirm

---

## Best Practices

### Before Calibration:
✅ Let sensor stabilize (15-30 minutes)  
✅ Ensure stable environment (no drafts)  
✅ Temperature: 20-25°C ideal  
✅ Humidity: 30-60% RH ideal  
✅ Keep sensor stationary  
✅ Avoid touching sensor  

### During Operation:
✅ Do not move device  
✅ Avoid temperature changes  
✅ Keep away from heat sources  
✅ Ensure good ventilation  
✅ Monitor Serial output for issues  

### After Operation:
✅ Allow cooldown (already built in)  
✅ Verify readings in View Sensor Info  
✅ Document calibration date  
✅ Note any issues observed  
✅ Test sensor in target environment  

---

## Maintenance Schedule

### Recommended Frequency:

| Operation | Frequency | When |
|-----------|-----------|------|
| **Condensation Removal** | As needed | When humidity stuck at 99-100% |
| **Offset Correction** | Annually | Once per year or when drift suspected |
| **View Sensor Info** | Daily/Weekly | Regular monitoring |
| **Reset Offsets** | Rarely | Only when needed |

---

## Comparison to Full System

| Feature | Full System | Utility Program |
|---------|-------------|-----------------|
| **HDC Sensors** | Up to 8 (multiplexed) | 1 (direct) |
| **BME280 Support** | Yes | No |
| **GPS** | Yes | No |
| **SD Card Logging** | Yes | No |
| **Real-time Display** | Cycling through sensors | Always visible |
| **Menu System** | None | Full menu |
| **Maintenance Ops** | Batch (all sensors) | Single sensor |
| **Purpose** | Data logging | Calibration/maintenance |
| **Size** | 68 KB | 28 KB |
| **Complexity** | High | Low |

---

## Use Cases

### Ideal For:
✅ **Bench testing** individual sensors  
✅ **Pre-deployment calibration**  
✅ **Service and repair** operations  
✅ **Quality control** checks  
✅ **Field calibration** with portable setup  
✅ **Training** on HDC maintenance  
✅ **Troubleshooting** problematic sensors  

### Not Ideal For:
❌ Multi-sensor deployments (use full system)  
❌ Continuous data logging (use full system)  
❌ GPS-tagged measurements (use full system)  
❌ Unattended operation (no automation)  

---

## File Information

**Filename:** `hdc_maintenance_utility.ino`  
**Version:** 1.0  
**File Size:** ~28 KB  
**Lines of Code:** ~940  
**Target Board:** Adafruit Feather M0 (or compatible)  
**Dependencies:**
- Adafruit_HDC302x library
- Adafruit_SH110X library (for OLED)
- Adafruit_GFX library
- Wire library (I2C)

---

## Compile and Upload

### Arduino IDE:
1. Install required libraries via Library Manager
2. Select **Tools → Board → Adafruit Feather M0**
3. Select **Tools → Port** (your device)
4. Click **Upload**

### Platform IO:
Add to `platformio.ini`:
```ini
[env:adafruit_feather_m0]
platform = atmelsam
board = adafruit_feather_m0
framework = arduino
lib_deps = 
    adafruit/Adafruit HDC302x
    adafruit/Adafruit SH110X
    adafruit/Adafruit GFX Library
```

---

## Summary

This standalone utility provides a **focused, user-friendly interface** for HDC302x sensor maintenance. With its simple menu system and real-time OLED feedback, it's perfect for:

- 🔧 Bench calibration
- 🧪 Quality control
- 🔍 Troubleshooting
- 📊 Pre-deployment testing
- 🛠️ Field service

**Simple, effective, and purpose-built for HDC sensor maintenance!** 🎉
