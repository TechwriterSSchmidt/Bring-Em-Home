// ESP32-S3-LCD-1.47 - Bring Em Home Project
// Using Arduino_GFX library with ST7789 driver
// Hardware: Waveshare ESP32-S3-LCD-1.47 (SKU 30558)

#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// RGB565 Color definitions
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define YELLOW  0xFFE0
#define CYAN    0x07FF
#define MAGENTA 0xF81F

// ESP32-S3-LCD-1.47 Pin Configuration
#define TFT_BL    46
#define TFT_RST   47 // GFX Library Reset Pin
#define LCD_RST   40 // Manual Reset Pin (from example)
#define TFT_DC    45
#define TFT_CS    21
#define TFT_SCK   38
#define TFT_MOSI  39

// Display dimensions
#define SCREEN_WIDTH  172
#define SCREEN_HEIGHT 320

// Create display bus and display object
Arduino_DataBus *bus = new Arduino_ESP32SPI(
    TFT_DC,   // DC
    TFT_CS,   // CS
    TFT_SCK,  // SCK
    TFT_MOSI, // MOSI
    -1        // MISO (not used)
);

// ST7789 with IPS=false (from demo)
Arduino_GFX *gfx = new Arduino_ST7789(
    bus,
    TFT_RST,      // RST
    0,            // rotation (0 = portrait)
    false,        // IPS panel
    SCREEN_WIDTH, // width = 172
    SCREEN_HEIGHT,// height = 320
    34,           // col_offset = 34
    0,            // row_offset = 0
    34,           // col_offset2
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
    // Manual Reset of LCD_RST (GPIO 40) as per example
    pinMode(LCD_RST, OUTPUT);
    digitalWrite(LCD_RST, LOW);
    delay(10);
    digitalWrite(LCD_RST, HIGH);

    Serial.begin(115200);
    delay(2000);
    Serial.println("

=== ESP32-S3 Bring Em Home ===");
    
    // Backlight Control
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH); // ON
    Serial.println("Backlight ON");
    
    // Initialize display
    Serial.println("Initializing display...");
    if (!gfx->begin()) {
        Serial.println("ERROR: gfx->begin() failed!");
        while(1) { delay(1000); }
    }
    
    // Apply specific init sequence
    lcd_reg_init();
    
    Serial.println("Display initialized OK");
    
    // Clear screen to black
    gfx->fillScreen(BLACK);
    delay(500);
    
    // Test Colors
    gfx->fillScreen(RED);
    delay(500);
    gfx->fillScreen(GREEN);
    delay(500);
    gfx->fillScreen(BLUE);
    delay(500);
    gfx->fillScreen(BLACK);
    
    // Text Test
    gfx->setCursor(10, 60);
    gfx->setTextColor(WHITE);
    gfx->setTextSize(2);
    gfx->println("Bring Em Home");
    
    gfx->setCursor(10, 100);
    gfx->setTextColor(YELLOW);
    gfx->setTextSize(1);
    gfx->println("ESP32-S3-LCD-1.47");
    
    gfx->setCursor(10, 120);
    gfx->setTextColor(CYAN);
    gfx->println("172x320 ST7789");
}

void loop() {
    // Simple animation or status update
    static int count = 0;
    gfx->fillRect(0, 200, SCREEN_WIDTH, 30, BLACK);
    gfx->setCursor(10, 210);
    gfx->setTextColor(GREEN);
    gfx->setTextSize(2);
    gfx->print("Count: ");
    gfx->print(count++);
    
    delay(1000);
}