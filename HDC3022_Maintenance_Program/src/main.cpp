#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_HDC302x.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// ============================================================================
// HDC SENSOR MAINTENANCE UTILITY
// Standalone program for HDC302x calibration and maintenance
// ============================================================================

// Hardware definitions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define BUTTON_A 9
#define BUTTON_B 6
#define BUTTON_C 5

// I2C addresses to try for HDC sensor
#define HDC_ADDR_PRIMARY 0x44
#define HDC_ADDR_SECONDARY 0x45

// Initialize objects
Adafruit_HDC302x hdc;
Adafruit_SH1107 display(SCREEN_HEIGHT, SCREEN_WIDTH, &Wire, OLED_RESET, 1000000, 100000);

// Menu states
enum MenuState {
  MENU_MAIN = 0,
  MENU_SENSOR_INFO = 1,
  MENU_CONDENSATION = 2,
  MENU_OFFSET_CORRECTION = 3,
  MENU_RESET_OFFSETS = 4,
  MENU_RUNNING_OPERATION = 5
};

MenuState currentMenu = MENU_MAIN;
uint8_t menuSelection = 0;
const uint8_t MENU_ITEMS = 4;

// Button state variables
bool buttonAPressed = false;
bool buttonBPressed = false;
bool buttonCPressed = false;
bool buttonALastState = false;
bool buttonBLastState = false;
bool buttonCLastState = false;
unsigned long lastButtonPress = 0;
#define BUTTON_DEBOUNCE 200

// Sensor data
struct SensorInfo {
  bool connected;
  uint8_t i2c_address;
  uint64_t nist_id;
  double temperature;
  double humidity;
  double temp_offset;
  double humidity_offset;
  bool heater_on;
  unsigned long last_reading;
};

SensorInfo sensor;

// Display update timing
unsigned long lastDisplayUpdate = 0;
#define DISPLAY_UPDATE_INTERVAL 500

// Function prototypes
void initializeSensor();
void readSensorData();
void readNISTID();
void readCurrentOffsets();
void updateDisplay();
void handleButtons();
void displayMainMenu();
void displaySensorInfo();
void displayConfirmation(String operation);
void runCondensationRemoval();
void runOffsetCorrection();
void resetOffsets();
bool performCondensationRemoval(double& finalTemp, double& finalHumidity);
bool performOffsetErrorCorrection(double& tempOffset, double& humidityOffset);
void updateOperationDisplay(String title, String status, float temp, float humidity, float tempRise, int elapsedSec);

// ============================================================================
// SETUP
// ============================================================================

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== HDC Sensor Maintenance Utility ===");
  Serial.println("Version 1.0");
  
  // Initialize I2C
  Wire.begin();
  
  // Initialize buttons
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  
  // Initialize display
  if(!display.begin(0x3C, true)) {
    Serial.println(F("SH1107 allocation failed"));
    while(1) delay(100);
  }
  
  display.setRotation(1);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("HDC Maintenance");
  display.println("Utility v1.0");
  display.println();
  display.println("Initializing...");
  display.display();
  delay(1500);
  
  // Initialize sensor
  initializeSensor();
  
  if (!sensor.connected) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("ERROR:");
    display.println();
    display.println("No HDC sensor");
    display.println("detected!");
    display.println();
    display.println("Check:");
    display.println("- I2C wiring");
    display.println("- Power supply");
    display.display();
    
    Serial.println("ERROR: No HDC sensor found!");
    Serial.println("Checked addresses 0x44 and 0x45");
    
    while(1) {
      delay(1000);
    }
  }
  
  // Read NIST ID and initial offsets
  readNISTID();
  readCurrentOffsets();
  
  // Display success
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("HDC302x Found!");
  display.print("Address: 0x");
  display.println(sensor.i2c_address, HEX);
  display.println();
  display.print("NIST ID:");
  // Format as 6-byte (48-bit) hex value: XX:XX:XX:XX:XX:XX
  char nistStr[18];
  sprintf(nistStr, "%02X%02X%02X%02X%02X%02X",
          (uint8_t)((sensor.nist_id >> 40) & 0xFF),
          (uint8_t)((sensor.nist_id >> 32) & 0xFF),
          (uint8_t)((sensor.nist_id >> 24) & 0xFF),
          (uint8_t)((sensor.nist_id >> 16) & 0xFF),
          (uint8_t)((sensor.nist_id >> 8) & 0xFF),
          (uint8_t)(sensor.nist_id & 0xFF));
  display.println(nistStr);
  display.println();
  display.println("Ready!");
  display.display();
  
  Serial.println("Sensor initialized successfully!");
  Serial.print("I2C Address: 0x");
  Serial.println(sensor.i2c_address, HEX);
  Serial.print("NIST ID: 0x");
  Serial.println(nistStr);
  
  delay(2000);
  
  lastDisplayUpdate = millis();
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
  unsigned long currentMillis = millis();
  
  // Read sensor data periodically
  if (currentMillis - sensor.last_reading >= 1000) {
    readSensorData();
    sensor.last_reading = currentMillis;
  }
  
  // Handle button presses
  handleButtons();
  
  // Update display
  if (currentMillis - lastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL) {
    updateDisplay();
    lastDisplayUpdate = currentMillis;
  }
}

// ============================================================================
// SENSOR FUNCTIONS
// ============================================================================

void initializeSensor() {
  sensor.connected = false;
  sensor.i2c_address = 0;
  sensor.nist_id = 0;
  sensor.heater_on = false;
  
  // Try primary address
  if (hdc.begin(HDC_ADDR_PRIMARY, &Wire)) {
    sensor.connected = true;
    sensor.i2c_address = HDC_ADDR_PRIMARY;
    Serial.println("HDC found at 0x44");
    return;
  }
  
  // Try secondary address
  if (hdc.begin(HDC_ADDR_SECONDARY, &Wire)) {
    sensor.connected = true;
    sensor.i2c_address = HDC_ADDR_SECONDARY;
    Serial.println("HDC found at 0x45");
    return;
  }
  
  Serial.println("HDC not found at 0x44 or 0x45");
}

void readSensorData() {
  if (!sensor.connected) return;
  
  double temp, humidity;
  if (hdc.readTemperatureHumidityOnDemand(temp, humidity, TRIGGERMODE_LP0)) {
    sensor.temperature = temp;
    sensor.humidity = humidity;
  }
}

void readNISTID() {
  sensor.nist_id = 0;
  
  // Read NIST ID directly from the sensor using the library function
  uint8_t nist_bytes[6];
  if (hdc.readNISTID(nist_bytes)) {
    // Convert 6-byte array to uint64_t
    sensor.nist_id = 0;
    for (int i = 0; i < 6; i++) {
      sensor.nist_id |= ((uint64_t)nist_bytes[i]) << ((5 - i) * 8);
    }
    
    Serial.print("Successfully read NIST ID from sensor: 0x");
    // Display as 6-byte (48-bit) hex value
    for (int i = 0; i < 6; i++) {
      if (nist_bytes[i] < 0x10) Serial.print("0");
      Serial.print(nist_bytes[i], HEX);
    }
    Serial.println();
  } else {
    Serial.println("ERROR: Failed to read NIST ID from sensor");
    sensor.nist_id = 0;
  }
}

void readCurrentOffsets() {
  if (hdc.readOffsets(sensor.temp_offset, sensor.humidity_offset)) {
    Serial.print("Current offsets - Temp: ");
    Serial.print(sensor.temp_offset);
    Serial.print("°C, RH: ");
    Serial.print(sensor.humidity_offset);
    Serial.println("%");
  } else {
    sensor.temp_offset = 0.0;
    sensor.humidity_offset = 0.0;
    Serial.println("Could not read current offsets");
  }
}

// ============================================================================
// DISPLAY FUNCTIONS
// ============================================================================

void updateDisplay() {
  switch (currentMenu) {
    case MENU_MAIN:
      displayMainMenu();
      break;
    case MENU_SENSOR_INFO:
      displaySensorInfo();
      break;
    case MENU_RUNNING_OPERATION:
      // Display is updated by operation functions
      break;
    default:
      displayMainMenu();
      break;
  }
}

void displayMainMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  // Title
  display.println("=== MAIN MENU ===");
  display.println();
  
  // Menu items
  const char* menuItems[] = {
    "1.View Sensor Info",
    "2.Condensation Rem.",
    "3.Offset Correction",
    "4.Reset Offsets"
  };
  
  for (uint8_t i = 0; i < MENU_ITEMS; i++) {
    if (i == menuSelection) {
      display.print("> ");
    } else {
      display.print("  ");
    }
    display.println(menuItems[i]);
  }
  
  // Current values footer
  display.setCursor(0, 56);
  display.print("T:");
  display.print(sensor.temperature, 1);
  display.print("C RH:");
  display.print(sensor.humidity, 1);
  display.print("%");
  
  display.display();
}

void displaySensorInfo() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  // Title
  display.println("SENSOR INFO");
  display.println("------------");
  
  // NIST ID - display as 6-byte (48-bit) hex value
  display.print("ID:");
  char nistStr[13];  // 12 hex digits + null terminator
  sprintf(nistStr, "%02X%02X%02X%02X%02X%02X",
          (uint8_t)((sensor.nist_id >> 40) & 0xFF),
          (uint8_t)((sensor.nist_id >> 32) & 0xFF),
          (uint8_t)((sensor.nist_id >> 24) & 0xFF),
          (uint8_t)((sensor.nist_id >> 16) & 0xFF),
          (uint8_t)((sensor.nist_id >> 8) & 0xFF),
          (uint8_t)(sensor.nist_id & 0xFF));
  
  // Display on one or two lines based on length
  if (strlen(nistStr) > 10) {
    // Display first 6 digits on first line
    char line1[7];
    strncpy(line1, nistStr, 6);
    line1[6] = '\0';
    display.println(line1);
    
    // Display remaining 6 digits on second line with indent
    display.print("  ");
    display.println(&nistStr[6]);
  } else {
    display.println(nistStr);
  }
  
  // Current readings
  display.print("Temp: ");
  display.print(sensor.temperature, 1);
  display.println("C");
  
  display.print("RH:   ");
  display.print(sensor.humidity, 1);
  display.println("%");
  
  // Current offsets
  display.println("Offsets:");
  display.print(" T:");
  display.print(sensor.temp_offset, 2);
  display.print(" RH:");
  display.println(sensor.humidity_offset, 2);
  
  // Footer
  display.setCursor(0, 56);
  display.print("Press C to exit");
  
  display.display();
}

void displayConfirmation(String operation) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  display.println(operation);
  display.println();
  display.println("This will take");
  display.println("several minutes.");
  display.println();
  display.println("A: Confirm");
  display.println("C: Cancel");
  
  display.display();
}

void updateOperationDisplay(String title, String status, float temp, float humidity, float tempRise, int elapsedSec) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  // Line 1: Title
  display.println(title);
  display.println("------------");
  
  // Line 2: Temperature with rise
  display.print("T:");
  display.print(temp, 1);
  display.print("C");
  if (tempRise > 0) {
    display.print(" (+");
    display.print(tempRise, 1);
    display.print(")");
  }
  display.println();
  
  // Line 3: Humidity
  display.print("RH:");
  display.print(humidity, 1);
  display.println("%");
  
  // Line 4: Time elapsed
  if (elapsedSec > 0) {
    display.print("Time:");
    display.print(elapsedSec);
    display.println("s");
  }
  display.println();
  
  // Line 5: Status
  display.println(status);
  
  display.display();
}

// ============================================================================
// BUTTON HANDLING
// ============================================================================

void handleButtons() {
  // Read current button states (buttons are active LOW)
  bool buttonAState = !digitalRead(BUTTON_A);
  bool buttonBState = !digitalRead(BUTTON_B);
  bool buttonCState = !digitalRead(BUTTON_C);
  
  // Debounce
  if (millis() - lastButtonPress < BUTTON_DEBOUNCE) {
    buttonALastState = buttonAState;
    buttonBLastState = buttonBState;
    buttonCLastState = buttonCState;
    return;
  }
  
  // Detect button press edges
  bool buttonAEdge = buttonAState && !buttonALastState;
  bool buttonBEdge = buttonBState && !buttonBLastState;
  bool buttonCEdge = buttonCState && !buttonCLastState;
  
  // Update last states
  buttonALastState = buttonAState;
  buttonBLastState = buttonBState;
  buttonCLastState = buttonCState;
  
  // Handle based on current menu
  switch (currentMenu) {
    case MENU_MAIN:
      if (buttonAEdge) {
        // Navigate up
        if (menuSelection > 0) {
          menuSelection--;
        }
        lastButtonPress = millis();
      }
      
      if (buttonCEdge) {
        // Navigate down
        if (menuSelection < MENU_ITEMS - 1) {
          menuSelection++;
        }
        lastButtonPress = millis();
      }
      
      if (buttonBEdge) {
        // Select menu item
        lastButtonPress = millis();
        
        switch (menuSelection) {
          case 0: // View Sensor Info
            currentMenu = MENU_SENSOR_INFO;
            break;
            
          case 1: // Condensation Removal
            displayConfirmation("CONDENSATION\nREMOVAL");
            currentMenu = MENU_CONDENSATION;
            break;
            
          case 2: // Offset Correction
            displayConfirmation("OFFSET ERROR\nCORRECTION");
            currentMenu = MENU_OFFSET_CORRECTION;
            break;
            
          case 3: // Reset Offsets
            displayConfirmation("RESET OFFSETS\nTO ZERO");
            currentMenu = MENU_RESET_OFFSETS;
            break;
        }
      }
      break;
      
    case MENU_SENSOR_INFO:
      if (buttonCEdge) {
        // Exit back to main menu
        currentMenu = MENU_MAIN;
        lastButtonPress = millis();
      }
      break;
      
    case MENU_CONDENSATION:
      if (buttonAEdge) {
        // Confirm - run condensation removal
        currentMenu = MENU_RUNNING_OPERATION;
        runCondensationRemoval();
        currentMenu = MENU_MAIN;
        lastButtonPress = millis();
      }
      
      if (buttonCEdge) {
        // Cancel
        currentMenu = MENU_MAIN;
        lastButtonPress = millis();
      }
      break;
      
    case MENU_OFFSET_CORRECTION:
      if (buttonAEdge) {
        // Confirm - run offset correction
        currentMenu = MENU_RUNNING_OPERATION;
        runOffsetCorrection();
        currentMenu = MENU_MAIN;
        lastButtonPress = millis();
      }
      
      if (buttonCEdge) {
        // Cancel
        currentMenu = MENU_MAIN;
        lastButtonPress = millis();
      }
      break;
      
    case MENU_RESET_OFFSETS:
      if (buttonAEdge) {
        // Confirm - reset offsets
        resetOffsets();
        currentMenu = MENU_MAIN;
        lastButtonPress = millis();
      }
      
      if (buttonCEdge) {
        // Cancel
        currentMenu = MENU_MAIN;
        lastButtonPress = millis();
      }
      break;
      
    case MENU_RUNNING_OPERATION:
      // No button handling during operation - operation functions manage their own flow
      break;
  }
}

// ============================================================================
// MAINTENANCE OPERATIONS
// ============================================================================

void runCondensationRemoval() {
  Serial.println("\n=== Starting Condensation Removal ===");
  
  double finalTemp, finalHumidity;
  bool success = performCondensationRemoval(finalTemp, finalHumidity);
  
  // Show results
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("CONDENSATION");
  display.println("REMOVAL");
  display.println();
  
  if (success) {
    display.println("SUCCESS!");
    display.println();
    display.print("Final T:");
    display.print(finalTemp, 1);
    display.println("C");
    display.print("Final RH:");
    display.print(finalHumidity, 1);
    display.println("%");
    Serial.println("Condensation removal completed successfully");
  } else {
    display.println("TIMEOUT");
    display.println();
    display.println("Humidity did not");
    display.println("reach < 1%");
    Serial.println("Condensation removal timed out");
  }
  
  display.println();
  display.print("Press any button");
  display.display();
  
  // Wait for button press
  while (digitalRead(BUTTON_A) == HIGH && 
         digitalRead(BUTTON_B) == HIGH && 
         digitalRead(BUTTON_C) == HIGH) {
    delay(10);
  }
  
  // Wait for release
  delay(300);
}

void runOffsetCorrection() {
  Serial.println("\n=== Starting Offset Error Correction ===");
  
  double tempOffset, humidityOffset;
  bool success = performOffsetErrorCorrection(tempOffset, humidityOffset);
  
  // Update stored offsets
  readCurrentOffsets();
  
  // Show results
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("OFFSET");
  display.println("CORRECTION");
  display.println();
  
  if (success) {
    display.println("SUCCESS!");
    display.println();
    display.print("Temp:");
    display.print(tempOffset, 2);
    display.println("C");
    display.print("RH:");
    display.print(humidityOffset, 2);
    display.println("%");
    display.println();
    display.println("Offsets saved");
    display.println("to sensor EEPROM");
    Serial.println("Offset correction completed successfully");
  } else {
    display.println("FAILED");
    display.println();
    display.println("Could not");
    display.println("complete offset");
    display.println("correction");
    Serial.println("Offset correction failed");
  }
  
  display.println();
  display.print("Press any button");
  display.display();
  
  // Wait for button press
  while (digitalRead(BUTTON_A) == HIGH && 
         digitalRead(BUTTON_B) == HIGH && 
         digitalRead(BUTTON_C) == HIGH) {
    delay(10);
  }
  
  // Wait for release
  delay(300);
}

void resetOffsets() {
  Serial.println("\n=== Resetting Offsets to Zero ===");
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("RESET OFFSETS");
  display.println();
  display.println("Writing 0.0 to");
  display.println("sensor EEPROM...");
  display.display();
  
  // Write zero offsets
  if (hdc.writeOffsets(0.0, 0.0)) {
    // Verify
    delay(100);
    double verifyTemp, verifyHum;
    if (hdc.readOffsets(verifyTemp, verifyHum)) {
      sensor.temp_offset = verifyTemp;
      sensor.humidity_offset = verifyHum;
      
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("RESET OFFSETS");
      display.println();
      display.println("SUCCESS!");
      display.println();
      display.print("Temp:");
      display.print(verifyTemp, 2);
      display.println("C");
      display.print("RH:");
      display.print(verifyHum, 2);
      display.println("%");
      display.println();
      display.print("Press any button");
      display.display();
      
      Serial.println("Offsets successfully reset");
      Serial.print("Verified - Temp: ");
      Serial.print(verifyTemp);
      Serial.print("°C, RH: ");
      Serial.print(verifyHum);
      Serial.println("%");
    }
  } else {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("RESET OFFSETS");
    display.println();
    display.println("FAILED!");
    display.println();
    display.println("Could not write");
    display.println("to sensor");
    display.println();
    display.print("Press any button");
    display.display();
    
    Serial.println("ERROR: Failed to write offsets");
  }
  
  // Wait for button press
  while (digitalRead(BUTTON_A) == HIGH && 
         digitalRead(BUTTON_B) == HIGH && 
         digitalRead(BUTTON_C) == HIGH) {
    delay(10);
  }
  
  // Wait for release
  delay(300);
}

// ============================================================================
// CORE MAINTENANCE FUNCTIONS
// ============================================================================

bool performCondensationRemoval(double& finalTemp, double& finalHumidity) {
  double initialTemp, initialHumidity;
  double currentTemp, currentHumidity;
  
  // Step 1: Read initial conditions
  if (!hdc.readTemperatureHumidityOnDemand(initialTemp, initialHumidity, TRIGGERMODE_LP0)) {
    Serial.println("Failed to read initial conditions");
    return false;
  }
  
  Serial.print("Initial Temp: ");
  Serial.print(initialTemp);
  Serial.print("°C, Initial RH: ");
  Serial.print(initialHumidity);
  Serial.println("%");
  
  // Step 2: Enable heater
  if (!hdc.heaterEnable(HEATER_HALF_POWER)) {
    Serial.println("Failed to enable heater");
    return false;
  }
  
  Serial.println("Heater enabled at HALF power");
  sensor.heater_on = true;
  
  // Step 3: Monitor until humidity < 1% or timeout
  unsigned long startTime = millis();
  unsigned long timeoutMs = 300000; // 5 minutes
  bool condensationRemoved = false;
  
  updateOperationDisplay("CONDENSATION", "Heating...", initialTemp, initialHumidity, 0, 0);
  
  while (millis() - startTime < timeoutMs) {
    delay(5000); // 5 seconds between readings
    
    if (!hdc.readTemperatureHumidityOnDemand(currentTemp, currentHumidity, TRIGGERMODE_LP0)) {
      Serial.println("Failed to read sensor during heating");
      continue;
    }
    
    float heatRise = currentTemp - initialTemp;
    int elapsedSec = (millis() - startTime) / 1000;
    
    Serial.print("Temp: ");
    Serial.print(currentTemp);
    Serial.print("°C (+");
    Serial.print(heatRise);
    Serial.print("°C), RH: ");
    Serial.print(currentHumidity);
    Serial.print("%, Time: ");
    Serial.print(elapsedSec);
    Serial.println("s");
    
    // Update display
    String status = "Heating...";
    if (currentHumidity < 10.0) {
      status = "Almost done!";
    }
    updateOperationDisplay("CONDENSATION", status, currentTemp, currentHumidity, heatRise, elapsedSec);
    
    // Check if condensation is removed
    if (currentHumidity < 1.0) {
      Serial.println("Condensation removed!");
      condensationRemoved = true;
      break;
    }
  }
  
  // Step 4: Disable heater
  hdc.heaterEnable(HEATER_OFF);
  sensor.heater_on = false;
  Serial.println("Heater disabled");
  
  if (!condensationRemoved) {
    Serial.println("WARNING: Timeout reached");
  }
  
  // Step 5: Cooldown
  Serial.println("Cooling down...");
  updateOperationDisplay("CONDENSATION", "Cooling...", currentTemp, currentHumidity, 0, 0);
  delay(10000);
  
  // Step 6: Read final conditions
  hdc.readTemperatureHumidityOnDemand(finalTemp, finalHumidity, TRIGGERMODE_LP0);
  
  Serial.print("Final Temp: ");
  Serial.print(finalTemp);
  Serial.print("°C, Final RH: ");
  Serial.print(finalHumidity);
  Serial.println("%");
  
  return condensationRemoved;
}

bool performOffsetErrorCorrection(double& tempOffset, double& humidityOffset) {
  // Look-Up Table for temperature rise
  const float LUT[8][4] = {
    {32.99, 30.94, 31.78, 31.92},  // 10% RH
    {36.36, 34.06, 36.43, 37.85},  // 15% RH  
    {40.33, 38.16, 41.44, 43.37},  // 20% RH
    {44.13, 42.31, 45.45, 46.89},  // 25% RH
    {47.14, 45.81, 48.01, 48.39},  // 30% RH
    {49.34, 48.52, 49.26, 48.99},  // 35% RH
    {50.29, 49.79, 49.83, 49.06},  // 40% RH
    {50.79, 50.34, 49.89, 49.06}   // 45% RH
  };
  
  double initialTemp, initialHumidity;
  double currentTemp, currentHumidity;
  
  // Step 1: Measure initial conditions
  if (!hdc.readTemperatureHumidityOnDemand(initialTemp, initialHumidity, TRIGGERMODE_LP0)) {
    Serial.println("Failed to read initial conditions");
    return false;
  }
  
  Serial.print("Initial Temp: ");
  Serial.print(initialTemp);
  Serial.print("°C, Initial RH: ");
  Serial.print(initialHumidity);
  Serial.println("%");
  
  // Step 2: Calculate target from LUT
  int tempCol = constrain((int)((initialTemp - 15.0) / 5.0), 0, 3);
  int humRow = constrain((int)((initialHumidity - 10.0) / 5.0), 0, 7);
  
  float targetTempRise = LUT[humRow][tempCol];
  
  Serial.print("Target temperature rise: ");
  Serial.print(targetTempRise);
  Serial.println("°C");
  
  // Step 3: Enable heater
  if (!hdc.heaterEnable(HEATER_FULL_POWER)) {
    Serial.println("Failed to enable heater");
    return false;
  }
  
  Serial.println("Heater enabled at FULL power");
  sensor.heater_on = true;
  
  // Step 4: Monitor until target temperature
  unsigned long startTime = millis();
  unsigned long timeoutMs = 120000; // 2 minutes
  float heatRise = 0.0;
  
  updateOperationDisplay("OFFSET CORR.", "Heating...", initialTemp, initialHumidity, 0, 0);
  
  while (millis() - startTime < timeoutMs) {
    delay(2000); // 2 seconds between readings
    
    if (!hdc.readTemperatureHumidityOnDemand(currentTemp, currentHumidity, TRIGGERMODE_LP0)) {
      Serial.println("Failed to read sensor during heating");
      continue;
    }
    
    heatRise = currentTemp - initialTemp;
    int elapsedSec = (millis() - startTime) / 1000;
    
    Serial.print("Temp: ");
    Serial.print(currentTemp);
    Serial.print("°C, Rise: ");
    Serial.print(heatRise);
    Serial.print("°C, RH: ");
    Serial.print(currentHumidity);
    Serial.println("%");
    
    // Update display
    String status = "Target:+";
    status += String(targetTempRise, 0);
    status += "C";
    updateOperationDisplay("OFFSET CORR.", status, currentTemp, currentHumidity, heatRise, elapsedSec);
    
    // Check if target reached
    if (heatRise >= targetTempRise) {
      Serial.println("Target temperature reached");
      break;
    }
  }
  
  // Step 5: Disable heater
  hdc.heaterEnable(HEATER_OFF);
  sensor.heater_on = false;
  Serial.println("Heater disabled");
  
  // Step 6: Calculate offsets
  humidityOffset = currentHumidity;
  tempOffset = 0.0;
  
  Serial.print("Calculated humidity offset: ");
  Serial.print(humidityOffset);
  Serial.println("% RH");
  
  // Step 7: Write offsets to sensor
  if (!hdc.writeOffsets(tempOffset, -humidityOffset)) {
    Serial.println("Failed to write offsets");
    return false;
  }
  
  Serial.println("Offsets written to sensor");
  
  // Verify
  double verifyTemp, verifyHum;
  if (hdc.readOffsets(verifyTemp, verifyHum)) {
    Serial.print("Verified offsets - Temp: ");
    Serial.print(verifyTemp);
    Serial.print("°C, RH: ");
    Serial.print(verifyHum);
    Serial.println("%");
  }
  
  // Step 8: Cooldown
  Serial.println("Cooling down...");
  updateOperationDisplay("OFFSET CORR.", "Cooling...", currentTemp, currentHumidity, 0, 0);
  delay(10000);
  
  // Step 9: Test corrected sensor
  double correctedTemp, correctedHumidity;
  if (hdc.readTemperatureHumidityOnDemand(correctedTemp, correctedHumidity, TRIGGERMODE_LP0)) {
    Serial.print("Corrected readings - Temp: ");
    Serial.print(correctedTemp);
    Serial.print("°C, RH: ");
    Serial.print(correctedHumidity);
    Serial.println("%");
  }
  
  return true;
}
