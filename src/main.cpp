#include <Arduino.h>
#include <TM1637Display.h>
#include <RTClib.h>
#include <Adafruit_NeoPixel.h>

#define CLK_PIN 2
#define DIO_PIN 3

#define LED_PIN 6
#define LED_COUNT 32

TM1637Display display(CLK_PIN, DIO_PIN);
RTC_DS1307 rtc;
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

const uint8_t SEG_DONE[] = {
	SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
	SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
	SEG_C | SEG_E | SEG_G,                           // n
	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
};

const uint8_t SEG_TRUE[] = {
  SEG_D | SEG_E | SEG_F | SEG_G,         // t
  SEG_E | SEG_G,                         // r
  SEG_C | SEG_D | SEG_E,                 // u
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G  // e
};

const uint8_t SEG_OUCH[] = {
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,// O
  SEG_C | SEG_D | SEG_E,                        // u
  SEG_D | SEG_E | SEG_G,                        // c
  SEG_C | SEG_E | SEG_F | SEG_G                 // h
};

// Function prototypes
void sevSegTest();
void colorWipe(uint32_t color, int wait);
void alarm(int durationMs);
// End of function prototypes

void setup() {
    Serial.begin(9600);
    Serial.println("MegaClock Starting...");
    Serial.println("Initializing RTC...");

    if (rtc.begin()) {
        Serial.println("RTC Ok");
    } else {
        Serial.println("RTC not found.");
        while(1);
    }

    if (rtc.isrunning()) {
        Serial.println("RTC is running.");
    } else{
        Serial.println("RTC is NOT running, setting the time!");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    Serial.println("RTC Initialized.");
    //rtc.adjust(DateTime(2024, 11, 4, 21, 35, 0));

    Serial.println("Initializing 7-Segment Display...");
    display.setBrightness(0x0f); // Set maximum brightness
    Serial.println("7-Segment Display Initialized.");

    Serial.println("Initializing NeoPixel Strip...");
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    strip.setBrightness(50); // Set brightness (0-255)
    Serial.println("NeoPixel Strip Initialized.");
}

void loop() {
    // Main loop code here
    DateTime time = rtc.now();
    int hour = time.hour();
    int minute = time.minute();

    //Full Timestamp
    Serial.println(String("DateTime::TIMESTAMP_FULL:\t")+time.timestamp(DateTime::TIMESTAMP_FULL));
    Serial.println();

    display.showNumberDecEx(time.hour(), 0b01000000, true, 2, 0); // Display hours with leading zero
    display.showNumberDec(time.minute(), true, 2, 2); // Display minutes with leading zero

    if(hour == 4 && minute >= 15) {
        alarm(10000); // Alarm for 10 seconds
    }
}

void sevSegTest() {
    display.setSegments(SEG_DONE);
    delay(1000);
    display.setSegments(SEG_TRUE);
    delay(1000);
    display.setSegments(SEG_OUCH);
    delay(1000);
    display.clear();
    delay(1000);

    
    display.showNumberDec(1234, false, 4, 0);
    delay(1000);
    display.showNumberDec(56, true, 4, 0);
    delay(1000);
    display.showNumberDec(56, false, 4, 0);
    delay(1000);

    display.showNumberHexEx(0x1A3F, false, 4, 0); // Display hex with dots on 2nd and 4th digits
    delay(1000);
    display.showNumberHexEx(0xd1, 0, true);
    delay(1000);
    display.clear();
    display.showNumberHexEx(0xd1, 0, true, 2);
    delay(1000);
    display.clear();
}

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

void alarm(int durationMs) {
    unsigned long start = millis();
    while (millis() - start < durationMs) {
        colorWipe(strip.Color(255, 0, 0), 20); // Red wipe
        colorWipe(strip.Color(0, 0, 0), 20);   // Off wipe
    }
}