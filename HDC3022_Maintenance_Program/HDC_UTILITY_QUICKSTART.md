# HDC Maintenance Utility - Quick Start

## 🚀 Get Started in 5 Minutes!

---

## What You'll Build

A **portable HDC sensor calibration station** with:
- ✅ Live sensor display
- ✅ Easy menu navigation  
- ✅ Automated maintenance operations
- ✅ Professional calibration tools

---

## Hardware Checklist

```
□ Adafruit Feather M0 (or compatible Arduino)
□ HDC302x sensor (HDC3020/3021/3022)
□ OLED display (128x64, I2C, SH1107)
□ 3 push buttons
□ Breadboard or PCB
□ Jumper wires
□ USB cable (for programming)
```

---

## Wiring Diagram

### Simple ASCII Diagram:

```
        Feather M0
       ┌─────────┐
       │  3.3V   ├──┬── HDC VIN
       │   GND   ├──┼── HDC GND
       │   SDA   ├──┤
       │   SCL   ├──┤
       │         │  │
       │   Pin9  ├──── Button A ── GND
       │   Pin6  ├──── Button B ── GND
       │   Pin5  ├──── Button C ── GND
       └─────────┘  │
                    └── OLED (SDA, SCL, VIN, GND)
```

### Detailed Connections:

| From Feather | To Component | Notes |
|--------------|--------------|-------|
| **3.3V** | HDC VIN | Power |
| **3.3V** | OLED VIN | Power |
| **GND** | HDC GND | Ground |
| **GND** | OLED GND | Ground |
| **SDA** | HDC SDA | I2C Data |
| **SDA** | OLED SDA | I2C Data (shared bus) |
| **SCL** | HDC SCL | I2C Clock |
| **SCL** | OLED SCL | I2C Clock (shared bus) |
| **Pin 9** | Button A | One side, other to GND |
| **Pin 6** | Button B | One side, other to GND |
| **Pin 5** | Button C | One side, other to GND |

**Note:** HDC and OLED share the same I2C bus (SDA/SCL)

---

## Installation

### Step 1: Install Arduino IDE
Download from: https://www.arduino.cc/en/software

### Step 2: Install Board Support
1. Open Arduino IDE
2. **File → Preferences**
3. Add to "Additional Board Manager URLs":
   ```
   https://adafruit.github.io/arduino-board-index/package_adafruit_index.json
   ```
4. **Tools → Board → Boards Manager**
5. Search "Adafruit SAMD"
6. Install "Adafruit SAMD Boards"

### Step 3: Install Libraries
**Tools → Manage Libraries**, then search and install:
- Adafruit HDC302x
- Adafruit SH110X
- Adafruit GFX Library

### Step 4: Upload Program
1. Open `hdc_maintenance_utility.ino`
2. **Tools → Board → Adafruit Feather M0**
3. **Tools → Port** → Select your Feather
4. Click **Upload** ➜

---

## First Boot

### You should see:

**1. Initialization (2 seconds)**
```
┌────────────────────┐
│ HDC Maintenance    │
│ Utility v1.0       │
│                    │
│ Initializing...    │
└────────────────────┘
```

**2. Sensor Detection (2 seconds)**
```
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

**3. Main Menu**
```
┌────────────────────┐
│ === MAIN MENU ===  │
│                    │
│ > 1.View Sensor    │
│   2.Condensation   │
│   3.Offset Corr.   │
│   4.Reset Offsets  │
│                    │
│ T:23.5C RH:55.2%   │
└────────────────────┘
```

✅ **Success! You're ready to use it!**

---

## Basic Operations

### Navigation:
- **Button A** = Move UP ⬆️
- **Button B** = SELECT ✅
- **Button C** = Move DOWN ⬇️ or EXIT ❌

### Try These:

#### 1️⃣ **View Sensor Info** (30 seconds)
1. Main menu → "View Sensor Info" already highlighted
2. Press **Button B** to select
3. See live sensor data
4. Press **Button C** to exit

#### 2️⃣ **Run Condensation Removal** (5 minutes)
1. Navigate to "2.Condensation Rem." (press **C** once)
2. Press **Button B** to select
3. Confirmation screen appears
4. Press **Button A** to confirm
5. Watch real-time progress
6. Press any button when complete

#### 3️⃣ **Run Offset Correction** (2 minutes)
1. Navigate to "3.Offset Corr."
2. Press **Button B** to select
3. Press **Button A** to confirm
4. Watch calibration process
5. Press any button when complete

---

## Understanding the Display

### Main Menu Footer:
```
T:23.5C RH:55.2%
└─┬─┘    └──┬──┘
  │         └── Current humidity (updates live)
  └── Current temperature (updates live)
```

### During Operations:
```
CONDENSATION
------------
T:45.3C (+21.8)  ← Temperature and heat rise
RH:15.2%         ← Current humidity
Time:45s         ← Seconds elapsed

Heating...       ← Current status
```

---

## Quick Troubleshooting

### ❌ "No HDC sensor detected!"

**Check:**
1. ✅ HDC sensor connected to 3.3V and GND
2. ✅ SDA and SCL connected correctly
3. ✅ No loose wires
4. ✅ Sensor is HDC302x (not HDC1080)

**Fix:** Recheck all connections, then press reset button

---

### ❌ OLED shows nothing

**Check:**
1. ✅ OLED connected to 3.3V and GND
2. ✅ OLED I2C address is 0x3C
3. ✅ SDA and SCL connected

**Fix:** Try different OLED or check with I2C scanner

---

### ❌ Buttons don't work

**Check:**
1. ✅ Buttons connect pin to GND when pressed
2. ✅ Correct pins: A=9, B=6, C=5
3. ✅ No stuck buttons

**Fix:** Test buttons with multimeter or try different buttons

---

## Quick Reference Card

### Print and keep this handy:

```
╔═══════════════════════════════════════╗
║   HDC MAINTENANCE UTILITY v1.0        ║
╠═══════════════════════════════════════╣
║ BUTTONS:                              ║
║  A = UP/Previous                      ║
║  B = SELECT/Enter                     ║
║  C = DOWN/Next or EXIT                ║
╠═══════════════════════════════════════╣
║ MENU OPTIONS:                         ║
║  1. View Sensor Info                  ║
║     - NIST ID, values, offsets        ║
║  2. Condensation Removal (5 min)      ║
║     - Removes moisture, RH < 1%       ║
║  3. Offset Correction (2 min)         ║
║     - Annual calibration              ║
║  4. Reset Offsets (<1 sec)            ║
║     - Clear calibration               ║
╠═══════════════════════════════════════╣
║ WHEN TO USE:                          ║
║  Condensation: RH stuck at 99-100%    ║
║  Offset: Annual or drift suspected    ║
║  Reset: Starting fresh                ║
╠═══════════════════════════════════════╣
║ SERIAL MONITOR: 115200 baud           ║
║ I2C ADDRESSES:                        ║
║  HDC: 0x44 or 0x45                    ║
║  OLED: 0x3C                           ║
╚═══════════════════════════════════════╝
```

---

## 5-Minute Tutorial

### Follow Along:

**Minute 1: Power Up**
- Plug in USB
- Watch initialization
- Main menu appears

**Minute 2: View Sensor**
- Press **B** (already on View Sensor)
- See your sensor info
- Note the NIST ID
- Press **C** to exit

**Minute 3: Navigate Menu**
- Press **C** to move down
- Press **A** to move up
- Practice navigating

**Minute 4: Check Current Offsets**
- Go back to View Sensor
- Look at "Offsets:" line
- Should show current calibration
- Press **C** to exit

**Minute 5: Ready to Use!**
- You now know how to:
  - Navigate menus
  - View sensor info
  - Understand the display

✅ **You're ready for actual calibration operations!**

---

## Next Steps

### For Regular Use:
1. 📖 Read **HDC_UTILITY_GUIDE.md** for detailed operations
2. 🔧 Run condensation removal when needed
3. 📅 Schedule annual offset correction
4. 📊 Monitor sensor info regularly

### For Advanced Users:
1. ⚙️ Modify button pins in code if needed
2. 🎨 Customize display messages
3. ⏱️ Adjust timeout values
4. 📈 Log operations to external system

---

## Common Workflows

### **New Sensor Calibration:**
```
1. View Sensor Info → Note initial values
2. Reset Offsets → Start fresh
3. Offset Correction → Calibrate
4. View Sensor Info → Verify calibration
```

### **Moisture Problem:**
```
1. View Sensor Info → Check if RH = 99-100%
2. Condensation Removal → Dry sensor
3. View Sensor Info → Confirm normal readings
```

### **Annual Maintenance:**
```
1. View Sensor Info → Note current offsets
2. Offset Correction → Recalibrate
3. View Sensor Info → Verify new offsets
4. Document in maintenance log
```

---

## Safety Notes

⚠️ **Heater Operation:**
- Sensor will get hot (up to 70°C) during operations
- Do not touch sensor during operation
- Allow cooldown period (built-in)
- Ensure good ventilation

⚠️ **Electrical:**
- Use 3.3V only (not 5V)
- Check for shorts before powering on
- Use proper gauge wire
- Ensure good connections

✅ **Safe Operation:**
- Monitor Serial output for errors
- Stop operation if sensor behaves abnormally
- Keep away from flammable materials
- Do not leave unattended during operation

---

## Getting Help

### If Something Goes Wrong:

1. **Check Serial Monitor** (115200 baud)
   - Shows detailed diagnostics
   - Error messages appear here
   - Helpful for troubleshooting

2. **Verify Connections**
   - Power (3.3V, GND)
   - I2C (SDA, SCL)
   - Buttons (pins 9, 6, 5)

3. **Read the Guide**
   - **HDC_UTILITY_GUIDE.md** has detailed troubleshooting
   - Check "Troubleshooting" section
   - Follow step-by-step solutions

4. **Test Components Separately**
   - Try I2C scanner
   - Test buttons individually
   - Verify OLED alone

---

## Success Checklist

After following this guide, you should have:

✅ Compiled and uploaded the program  
✅ Seen the main menu on OLED  
✅ Navigated the menu with buttons  
✅ Viewed sensor information  
✅ Understood the display format  
✅ Know when to use each function  

**Congratulations! You're ready to use the HDC Maintenance Utility!** 🎉

---

## Resources

| Document | Purpose |
|----------|---------|
| **QUICK_START.md** | This file - Get started fast |
| **HDC_UTILITY_GUIDE.md** | Complete user manual |
| **hdc_maintenance_utility.ino** | The program code |

---

## Support

For questions or issues:
1. Check HDC_UTILITY_GUIDE.md troubleshooting
2. Review Serial Monitor output
3. Verify all connections
4. Test with known-good components

---

**You're all set! Time to calibrate some sensors!** 🔧📊✨
