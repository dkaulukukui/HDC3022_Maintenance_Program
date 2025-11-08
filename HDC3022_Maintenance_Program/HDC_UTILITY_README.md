# NEW: HDC Sensor Maintenance Utility

## 🎉 Standalone Calibration Tool Created!

I've created a **brand new, standalone program** specifically for HDC sensor maintenance. This is completely separate from the full environmental monitoring system.

---

## 📦 What You Get

### [hdc_maintenance_utility.ino](computer:///mnt/user-data/outputs/hdc_maintenance_utility.ino) (27 KB)
**The complete standalone program** - ready to upload!

### [HDC_UTILITY_QUICKSTART.md](computer:///mnt/user-data/outputs/HDC_UTILITY_QUICKSTART.md) ⭐ **Start Here!**
Get up and running in 5 minutes with:
- Wiring diagrams
- Installation steps
- First boot walkthrough
- 5-minute tutorial

### [HDC_UTILITY_GUIDE.md](computer:///mnt/user-data/outputs/HDC_UTILITY_GUIDE.md) 📖 **Complete Manual**
Comprehensive guide with:
- Full feature documentation
- Detailed operation instructions
- Troubleshooting guide
- Best practices

---

## 🎯 What Makes This Different?

### Full Environmental System:
- ❌ Requires: GPS, SD card, 8 sensors, multiplexer, BME280
- ✅ Purpose: Continuous data logging
- ⚙️ Complexity: High (68 KB, many features)
- 📊 Display: Cycles through sensors

### **NEW Maintenance Utility:**
- ✅ Requires: Just 1 HDC sensor, OLED, 3 buttons
- ✅ Purpose: Calibration and maintenance
- ⚙️ Complexity: Low (27 KB, focused)
- 📊 Display: Always shows current sensor

---

## 🔧 What It Does

### 1. View Sensor Info
Real-time display showing:
```
SENSOR INFO
------------
ID:5449000030220001  ← NIST ID
Temp: 23.5C          ← Live reading
RH:   55.2%          ← Live reading
Offsets:
 T:0.00 RH:-2.50    ← Current calibration

Press C to exit
```

### 2. Condensation Removal
Automated moisture removal:
```
CONDENSATION
------------
T:45.3C (+21.8)     ← Temperature + rise
RH:15.2%            ← Dropping humidity
Time:45s            ← Elapsed time

Heating...          ← Status
```
- Uses heater at **50% power**
- Runs until **humidity < 1%**
- **5 minute** timeout
- Live progress on OLED

### 3. Offset Error Correction
Professional calibration:
```
OFFSET CORR.
------------
T:68.2C (+44.7)     ← Temp + rise
RH:8.5%             ← Current RH
Time:65s            ← Elapsed

Target:+48C         ← Target from LUT
```
- Uses **Look-Up Table algorithm**
- **Saves to sensor EEPROM**
- **2 minute** timeout
- Shows target temperature

### 4. Reset Offsets
Clear calibration:
```
RESET OFFSETS

SUCCESS!

Temp:0.00C
RH:0.00%

Press any button
```
- Instant operation
- Writes 0.0 to sensor
- Verifies write success

---

## 🎮 Menu Navigation

### Main Menu:
```
=== MAIN MENU ===

> 1.View Sensor Info   ← Cursor (Button A/C to move)
  2.Condensation Rem.
  3.Offset Correction
  4.Reset Offsets

T:23.5C RH:55.2%      ← Live readings
```

### Button Controls:
| Button | Function |
|--------|----------|
| **A** | Navigate UP / Previous |
| **B** | SELECT / Enter |
| **C** | Navigate DOWN / Next / Exit |

### Navigation Flow:
```
Main Menu
    │
    ├──[B]─→ View Sensor Info ──[C]─→ Back to Menu
    │
    ├──[B]─→ Confirmation ──[A]─→ Run Operation ─→ Results ──[Any]─→ Menu
    │                    └──[C]─→ Cancel → Menu
    │
    └──[B]─→ ... (same for other options)
```

---

## 🚀 Quick Start (3 Steps!)

### Step 1: Wire It Up
```
Feather M0   →   HDC302x
  3.3V      →    VIN
  GND       →    GND
  SDA       →    SDA
  SCL       →    SCL

Feather M0   →   OLED (same I2C bus)
  SDA       →    SDA
  SCL       →    SCL
  3.3V      →    VIN
  GND       →    GND

Feather M0   →   Buttons
  Pin 9     →    Button A (other side to GND)
  Pin 6     →    Button B (other side to GND)
  Pin 5     →    Button C (other side to GND)
```

### Step 2: Upload Program
1. Install libraries: Adafruit_HDC302x, Adafruit_SH110X, Adafruit_GFX
2. Open `hdc_maintenance_utility.ino`
3. Select board: Adafruit Feather M0
4. Click Upload!

### Step 3: Use It!
```
Power on → Main menu appears → Navigate with A/C → Select with B → Done!
```

---

## 📊 Live Display Updates

### Updates in Real-Time:
- ✅ Main menu footer: Every 500ms
- ✅ Sensor info screen: Every 500ms
- ✅ During operations: Every 2-5 seconds
- ✅ Temperature and humidity always current

### Serial Monitor (115200 baud):
Provides detailed diagnostics:
```
=== HDC Sensor Maintenance Utility ===
Version 1.0
HDC found at 0x44
Using hardware serial number
Current offsets - Temp: 0.00°C, RH: -2.50%
Sensor initialized successfully!
```

---

## 🔍 Key Features

### Automatic NIST ID Reading
Tries three methods:
1. **Hardware serial number** (40-bit from register 0xFB)
2. **Manufacturer + Device ID** (registers 0xFC, 0xFE)
3. **Placeholder** (if I2C fails)

### Offset Management
- **Read** current offsets from sensor
- **Write** new offsets to sensor EEPROM
- **Verify** offsets after writing
- **Reset** to zero when needed

### Error Handling
- Detects if sensor not connected
- Shows clear error messages
- Validates all operations
- Confirms before destructive actions

### User-Friendly
- Clear menu system
- Real-time feedback
- Confirmation dialogs
- Status messages
- Progress indicators

---

## 🎯 Use Cases

### Perfect For:
✅ **Bench testing** individual sensors  
✅ **Pre-deployment** calibration  
✅ **Field service** with portable setup  
✅ **Quality control** testing  
✅ **Training** on HDC maintenance  
✅ **Troubleshooting** sensor issues  
✅ **Regular maintenance** schedule  

### Not Designed For:
❌ Multi-sensor systems (use full system)  
❌ Continuous logging (use full system)  
❌ GPS-tagged data (use full system)  
❌ Unattended operation  

---

## 📏 Technical Specs

### Program:
- **Size:** 27 KB (40% of full system)
- **RAM:** ~2 KB
- **Flash:** ~25 KB
- **Dependencies:** 3 libraries
- **Lines of Code:** ~940

### Operations:
| Operation | Duration | Success Criteria |
|-----------|----------|------------------|
| Condensation | 30s - 5min | Humidity < 1% |
| Offset Correction | 1-2 min | Target temp reached |
| Reset Offsets | <1 second | Write verified |
| View Info | Instant | Always available |

### Hardware:
- **Minimum:** Feather M0, HDC, OLED, 3 buttons
- **Power:** USB or battery (3.3V)
- **I2C:** Single bus for HDC + OLED
- **GPIO:** 3 pins for buttons

---

## 🆚 Comparison Table

| Feature | Full System | Utility | Winner |
|---------|-------------|---------|---------|
| **Setup Complexity** | High (8+ sensors) | Low (1 sensor) | 🏆 Utility |
| **Code Size** | 68 KB | 27 KB | 🏆 Utility |
| **Menu System** | No | Yes | 🏆 Utility |
| **Real-time Display** | Cycles | Always visible | 🏆 Utility |
| **Data Logging** | GPS + SD | None | 🏆 Full |
| **Multi-Sensor** | Yes (8 HDC) | No | 🏆 Full |
| **Purpose** | Deployment | Maintenance | Different |

**Bottom Line:** Use the right tool for the job!
- Deployment/logging → Full system
- Calibration/testing → Utility

---

## 📚 Documentation Included

### For New Users:
1. **HDC_UTILITY_QUICKSTART.md** - 5-minute setup
   - Hardware checklist
   - Wiring diagram
   - First boot guide
   - Basic tutorial

### For All Users:
2. **HDC_UTILITY_GUIDE.md** - Complete manual
   - All features explained
   - Step-by-step instructions
   - Troubleshooting
   - Best practices
   - Technical specs

### Supporting Files:
3. **hdc_maintenance_utility.ino** - The program
   - Well-commented code
   - Easy to customize
   - Modular design

---

## 🎓 Learning Path

### Beginner (Day 1):
1. Read QUICKSTART
2. Wire hardware
3. Upload program
4. Navigate menus
5. View sensor info

### Intermediate (Week 1):
1. Run condensation removal
2. Understand OLED displays
3. Read Serial Monitor
4. Interpret results

### Advanced (Month 1):
1. Run offset correction
2. Understand LUT algorithm
3. Schedule maintenance
4. Document calibrations
5. Customize code

---

## ⚙️ Customization Options

Easy modifications you can make:

### Button Pins:
```cpp
#define BUTTON_A 9   // Change pin number
#define BUTTON_B 6
#define BUTTON_C 5
```

### Display Update Rate:
```cpp
#define DISPLAY_UPDATE_INTERVAL 500  // Change to 1000 for slower updates
```

### Timeout Values:
```cpp
unsigned long timeoutMs = 300000;  // Condensation: 5 min
unsigned long timeoutMs = 120000;  // Offset: 2 min
```

### Criteria:
```cpp
if (currentHumidity < 1.0) {  // Change threshold
```

---

## 🛡️ Safety Features

### Built-in Protections:
✅ **Confirmation dialogs** - Prevents accidents  
✅ **Timeout protection** - Won't run forever  
✅ **Cooldown periods** - Protects sensor  
✅ **Status monitoring** - Shows what's happening  
✅ **Error detection** - Catches problems early  

### User Safety:
⚠️ **Heater warning** - Sensor gets hot (up to 70°C)  
⚠️ **Cooldown built-in** - 10 seconds after heating  
⚠️ **Ventilation needed** - Keep airflow around sensor  
⚠️ **No touching** - Don't touch sensor during operation  

---

## 📈 What's Next?

### Immediate:
1. ✅ Upload program
2. ✅ Test all functions
3. ✅ Verify sensor readings
4. ✅ Run first calibration

### Short-term:
1. 📝 Document calibration date
2. 📊 Track offset values
3. 🔧 Set maintenance schedule
4. 📸 Save OLED screenshots

### Long-term:
1. 🎓 Train team on utility
2. 📋 Create calibration procedures
3. 📅 Schedule annual maintenance
4. 🔬 Validate in target environment

---

## 🎉 Summary

### What You Have Now:

✅ **Standalone calibration utility** (27 KB)  
✅ **Easy menu navigation** (A/B/C buttons)  
✅ **Real-time sensor display** (always visible)  
✅ **Professional operations** (3 maintenance functions)  
✅ **Complete documentation** (quick start + full guide)  
✅ **Simple hardware** (just 1 sensor + OLED)  

### What You Can Do:

🔧 Calibrate HDC sensors professionally  
🧪 Test sensors before deployment  
📊 Monitor sensor health  
🛠️ Service sensors in the field  
📋 Maintain calibration schedule  
✨ Ensure accurate measurements  

---

## 💡 Pro Tips

### Best Results:
1. Let sensor stabilize 15-30 minutes before calibration
2. Maintain stable environment during operations
3. Document all calibrations
4. Test in target environment after calibration
5. Schedule regular maintenance

### Common Mistakes to Avoid:
❌ Rushing - Let sensor stabilize first  
❌ Moving device during operation  
❌ Ignoring timeout warnings  
❌ Skipping cooldown periods  
❌ Not documenting calibrations  

---

## 🎯 Success Checklist

After using this utility, you should be able to:

✅ Navigate the menu system  
✅ View sensor information  
✅ Run condensation removal  
✅ Perform offset correction  
✅ Reset offsets when needed  
✅ Interpret OLED displays  
✅ Understand Serial Monitor output  
✅ Troubleshoot common issues  

**If you can do all these, you're a pro!** 🏆

---

## 📞 Getting Help

### If You Need Assistance:

1. **Read the Guides:**
   - QUICKSTART for setup
   - GUIDE for operations
   
2. **Check Serial Monitor:**
   - Shows detailed diagnostics
   - Error messages appear here
   
3. **Verify Hardware:**
   - Power connections
   - I2C wiring
   - Button connections
   
4. **Test Components:**
   - HDC sensor alone
   - OLED display alone
   - Buttons individually

---

## 🎊 You're All Set!

Everything you need for professional HDC sensor maintenance is ready:

📦 **Program:** hdc_maintenance_utility.ino  
📖 **Quick Start:** HDC_UTILITY_QUICKSTART.md  
📚 **Full Guide:** HDC_UTILITY_GUIDE.md  

**Time to build your calibration station and start maintaining those sensors!** 🚀🔧✨
