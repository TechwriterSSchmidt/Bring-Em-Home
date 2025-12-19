// ESP32-S3-LCD-1.3 - Bring Em Home Project
// Using Arduino_GFX library with ST7789 driver
// Hardware: Waveshare ESP32-S3-LCD-1.3 (240x240)
// Pinout based on nishad2m8/WS-1.3

#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <Wire.h>
#include <TinyGPS++.h>
#include <MechaQMC5883.h>
#include <Preferences.h>
#include <vector>

// RGB565 Color definitions
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define YELLOW  0xFFE0
#define CYAN    0x07FF
#define MAGENTA 0xF81F

// ESP32-S3-LCD-1.3 Pin Configuration
// Based on nishad2m8/WS-1.3 User_Setup.h
#define TFT_MOSI  41
#define TFT_SCK   40
#define TFT_CS    39
#define TFT_DC    38
#define TFT_RST   42
#define TFT_BL    45 
#define BUTTON_PIN 14 // External waterproof button

// Sensor Pins
#define GPS_RX 17
#define GPS_TX 18
#define I2C_SDA 8
#define I2C_SCL 9

// Display dimensions
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 240

// Power Management
#define DISPLAY_TIMEOUT 300000 // 5 minutes in milliseconds
unsigned long lastInteractionTime = 0;
bool isDisplayOn = true;
bool lastButtonState = HIGH;
unsigned long buttonPressStartTime = 0;
bool isLongPressHandled = false;

// Objects
TinyGPSPlus gps;
MechaQMC5883 qmc;
Preferences preferences;

// Navigation Data
double homeLat = 0.0;
double homeLon = 0.0;
bool hasHome = false;
int currentHeading = 0;

// Breadcrumbs
struct Breadcrumb {
    double lat;
    double lon;
};
std::vector<Breadcrumb> breadcrumbs;
unsigned long lastBreadcrumbTime = 0;
const unsigned long BREADCRUMB_INTERVAL = 150000; // 2.5 minutes

// Create display bus and display object
Arduino_DataBus *bus = new Arduino_ESP32SPI(
    TFT_DC,   // DC
    TFT_CS,   // CS
    TFT_SCK,  // SCK
    TFT_MOSI, // MOSI
    -1        // MISO (not used)
);

// ST7789 240x240
Arduino_GFX *gfx = new Arduino_ST7789(
    bus,
    TFT_RST,      // RST
    0,            // rotation (0 = portrait)
    true,         // IPS panel
    SCREEN_WIDTH, // width = 240
    SCREEN_HEIGHT,// height = 240
    0,            // col_offset1
    0,            // row_offset1
    0,            // col_offset2
    0             // row_offset2
);

// Initialization sequence from Waveshare Demo
void lcd_reg_init(void) {

  static const uint8_t init_operations[] = {
    BEGIN_WRITE,
    WRITE_COMMAND_8, 0x11,  // 2: Out of sleep mode, no args, w/delay
    END_WRITE,
    DELAY, 120,

    BEGIN_WRITE,
    WRITE_C8_D16, 0xDF, 0x98, 0x53,
    WRITE_C8_D8, 0xB2, 0x23, 

    WRITE_COMMAND_8, 0xB7,
    WRITE_BYTES, 4,
    0x00, 0x47, 0x00, 0x6F,

    WRITE_COMMAND_8, 0xBB,
    WRITE_BYTES, 6,
    0x1C, 0x1A, 0x55, 0x73, 0x63, 0xF0,

    WRITE_C8_D16, 0xC0, 0x44, 0xA4,
    WRITE_C8_D8, 0xC1, 0x16, 

    WRITE_COMMAND_8, 0xC3,
    WRITE_BYTES, 8,
    0x7D, 0x07, 0x14, 0x06, 0xCF, 0x71, 0x72, 0x77,

    WRITE_COMMAND_8, 0xC4,
    WRITE_BYTES, 12,
    0x00, 0x00, 0xA0, 0x79, 0x0B, 0x0A, 0x16, 0x79, 0x0B, 0x0A, 0x16, 0x82,

    WRITE_COMMAND_8, 0xC8,
    WRITE_BYTES, 32,
    0x3F, 0x32, 0x29, 0x29, 0x27, 0x2B, 0x27, 0x28, 0x28, 0x26, 0x25, 0x17, 0x12, 0x0D, 0x04, 0x00, 0x3F, 0x32, 0x29, 0x29, 0x27, 0x2B, 0x27, 0x28, 0x28, 0x26, 0x25, 0x17, 0x12, 0x0D, 0x04, 0x00,

    WRITE_COMMAND_8, 0xD0,
    WRITE_BYTES, 5,
    0x04, 0x06, 0x6B, 0x0F, 0x00,

    WRITE_C8_D16, 0xD7, 0x00, 0x30,
    WRITE_C8_D8, 0xE6, 0x14, 
    WRITE_C8_D8, 0xDE, 0x01, 

    WRITE_COMMAND_8, 0xB7,
    WRITE_BYTES, 5,
    0x03, 0x13, 0xEF, 0x35, 0x35,

    WRITE_COMMAND_8, 0xC1,
    WRITE_BYTES, 3,
    0x14, 0x15, 0xC0,

    WRITE_C8_D16, 0xC2, 0x06, 0x3A,
    WRITE_C8_D16, 0xC4, 0x72, 0x12,
    WRITE_C8_D8, 0xBE, 0x00, 
    WRITE_C8_D8, 0xDE, 0x02, 

    WRITE_COMMAND_8, 0xE5,
    WRITE_BYTES, 3,
    0x00, 0x02, 0x00,

    WRITE_COMMAND_8, 0xE5,
    WRITE_BYTES, 3,
    0x01, 0x02, 0x00,

    WRITE_C8_D8, 0xDE, 0x00, 
    WRITE_C8_D8, 0x35, 0x00, 
    WRITE_C8_D8, 0x3A, 0x05, 

    WRITE_COMMAND_8, 0x2A,
    WRITE_BYTES, 4,
    0x00, 0x22, 0x00, 0xCD,

    WRITE_COMMAND_8, 0x2B,
    WRITE_BYTES, 4,
    0x00, 0x00, 0x01, 0x3F,

    WRITE_C8_D8, 0xDE, 0x02, 

    WRITE_COMMAND_8, 0xE5,
    WRITE_BYTES, 3,
    0x00, 0x02, 0x00,
    
    WRITE_C8_D8, 0xDE, 0x00, 
    WRITE_C8_D8, 0x36, 0x00,
    WRITE_COMMAND_8, 0x21,
    END_WRITE,
    
    DELAY, 10,

    BEGIN_WRITE,
    WRITE_COMMAND_8, 0x29,  // 5: Main screen turn on, no args, w/delay
    END_WRITE
  };
  bus->batchOperation(init_operations, sizeof(init_operations));
}

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("\n\n=== ESP32-S3 Bring Em Home ===");
    
    // Backlight Control
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH); // ON
    
    // Button Setup
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    lastInteractionTime = millis();

    // Initialize Sensors
    Wire.begin(I2C_SDA, I2C_SCL);
    qmc.init();
    Serial1.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);

    // Initialize Preferences
    preferences.begin("bringemhome", false);
    homeLat = preferences.getDouble("lat", 0.0);
    homeLon = preferences.getDouble("lon", 0.0);
    hasHome = (homeLat != 0.0 && homeLon != 0.0);
    Serial.printf("Home: %f, %f\n", homeLat, homeLon);

    // Initialize display
    if (!gfx->begin()) {
        Serial.println("ERROR: gfx->begin() failed!");
        while(1) { delay(1000); }
    }
    
    gfx->fillScreen(BLACK);
    gfx->setCursor(10, 60);
    gfx->setTextColor(WHITE);
    gfx->setTextSize(2);
    gfx->println("Bring Em Home");
    
    gfx->setCursor(10, 100);
    gfx->setTextColor(YELLOW);
    gfx->setTextSize(1);
    gfx->println("Waiting for GPS...");
}

void loop() {
    // 1. Process GPS
    while (Serial1.available()) {
        gps.encode(Serial1.read());
    }

    // 2. Process Compass
    uint16_t x, y, z;
    float heading;
    qmc.read(&x, &y, &z, &heading);
    currentHeading = (int)heading;

    // 3. Button Logic
    bool currentButtonState = digitalRead(BUTTON_PIN);
    
    // Button Pressed
    if (currentButtonState == LOW && lastButtonState == HIGH) {
        buttonPressStartTime = millis();
        isLongPressHandled = false;
    }
    
    // Button Held
    if (currentButtonState == LOW) {
        if (!isLongPressHandled && (millis() - buttonPressStartTime > 2000)) {
            // Long Press: Save Home
            Serial.println("Long Press: Saving Home");
            if (gps.location.isValid()) {
                homeLat = gps.location.lat();
                homeLon = gps.location.lng();
                hasHome = true;
                preferences.putDouble("lat", homeLat);
                preferences.putDouble("lon", homeLon);
                
                // Visual Feedback
                if (!isDisplayOn) { digitalWrite(TFT_BL, HIGH); isDisplayOn = true; }
                gfx->fillScreen(GREEN);
                gfx->setCursor(20, 100);
                gfx->setTextColor(BLACK);
                gfx->setTextSize(2);
                gfx->println("HOME SAVED!");
                delay(1000);
                gfx->fillScreen(BLACK);
            } else {
                // Error Feedback
                if (!isDisplayOn) { digitalWrite(TFT_BL, HIGH); isDisplayOn = true; }
                gfx->fillScreen(RED);
                gfx->setCursor(20, 100);
                gfx->setTextColor(WHITE);
                gfx->setTextSize(2);
                gfx->println("NO GPS FIX!");
                delay(1000);
                gfx->fillScreen(BLACK);
            }
            isLongPressHandled = true;
            lastInteractionTime = millis();
        }
    }
    
    // Button Released
    if (currentButtonState == HIGH && lastButtonState == LOW) {
        if (!isLongPressHandled) {
            // Short Press: Toggle Display
            if (isDisplayOn) {
                digitalWrite(TFT_BL, LOW);
                isDisplayOn = false;
            } else {
                digitalWrite(TFT_BL, HIGH);
                isDisplayOn = true;
                lastInteractionTime = millis();
            }
        }
    }
    lastButtonState = currentButtonState;

    // 4. Breadcrumb Logic
    if (gps.location.isValid()) {
        if (millis() - lastBreadcrumbTime > BREADCRUMB_INTERVAL) {
            Breadcrumb b;
            b.lat = gps.location.lat();
            b.lon = gps.location.lng();
            breadcrumbs.push_back(b);
            lastBreadcrumbTime = millis();
            Serial.printf("Breadcrumb saved: %f, %f\n", b.lat, b.lon);
        }
    }

    // 5. Display Timeout
    if (isDisplayOn && (millis() - lastInteractionTime > DISPLAY_TIMEOUT)) {
        digitalWrite(TFT_BL, LOW);
        isDisplayOn = false;
    }

    // 6. Update Display
    if (isDisplayOn) {
        static unsigned long lastUpdate = 0;
        if (millis() - lastUpdate > 1000) {
            lastUpdate = millis();
            
            // Clear screen (or use partial updates for better performance)
            gfx->fillScreen(BLACK);
            
            // Header
            gfx->setCursor(0, 20);
            gfx->setTextColor(WHITE);
            gfx->setTextSize(2);
            if (gps.location.isValid()) {
                gfx->printf("Sats: %d", gps.satellites.value());
            } else {
                gfx->print("No GPS Fix");
            }
            
            // Heading
            gfx->setCursor(140, 20);
            gfx->setTextColor(CYAN);
            gfx->printf("%d deg", currentHeading);

            // Home Info
            if (hasHome && gps.location.isValid()) {
                double dist = gps.distanceBetween(gps.location.lat(), gps.location.lng(), homeLat, homeLon);
                double bearing = gps.courseTo(gps.location.lat(), gps.location.lng(), homeLat, homeLon);
                
                // Distance
                gfx->setCursor(20, 80);
                gfx->setTextColor(GREEN);
                gfx->setTextSize(3);
                if (dist < 1000) {
                    gfx->printf("%.0f m", dist);
                } else {
                    gfx->printf("%.2f km", dist / 1000.0);
                }
                
                // Direction Arrow (Simplified text for now)
                gfx->setCursor(20, 140);
                gfx->setTextColor(YELLOW);
                gfx->setTextSize(2);
                
                // Relative bearing
                int relBearing = (int)bearing - currentHeading;
                if (relBearing < 0) relBearing += 360;
                if (relBearing >= 360) relBearing -= 360;
                
                if (relBearing > 337 || relBearing <= 22) gfx->print("   ^   "); // Ahead
                else if (relBearing > 22 && relBearing <= 67) gfx->print("   /   "); // Right-Front
                else if (relBearing > 67 && relBearing <= 112) gfx->print("   >   "); // Right
                else if (relBearing > 112 && relBearing <= 157) gfx->print("   \\   "); // Right-Back
                else if (relBearing > 157 && relBearing <= 202) gfx->print("   v   "); // Back
                else if (relBearing > 202 && relBearing <= 247) gfx->print("   /   "); // Left-Back
                else if (relBearing > 247 && relBearing <= 292) gfx->print("   <   "); // Left
                else if (relBearing > 292 && relBearing <= 337) gfx->print("   \\   "); // Left-Front
                
            } else if (!hasHome) {
                gfx->setCursor(20, 100);
                gfx->setTextColor(RED);
                gfx->setTextSize(2);
                gfx->print("Set Home!");
            }
            
            // Breadcrumbs count
            gfx->setCursor(10, 220);
            gfx->setTextColor(MAGENTA);
            gfx->setTextSize(1);
            gfx->printf("Pts: %d", breadcrumbs.size());
        }
    }
    
    delay(10);
}