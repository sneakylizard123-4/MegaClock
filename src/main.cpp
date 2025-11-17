#include <Arduino.h>
#include <TM1637Display.h>
#include <RTClib.h>
#include <Adafruit_NeoPixel.h>

#define AMPM true // true for 12-hour format, false for 24-hour format
#if AMPM
  bool isPM;
  bool ampm = true;
#else
  // no need for isPM bool cos its 24h
  bool ampm = false;
#endif

#define CLK_PIN 2
#define DIO_PIN 3

#define NEO_PIN 6
#define NEO_COUNT 32

#define buzzer 11

TM1637Display display(CLK_PIN, DIO_PIN);
RTC_DS1307 rtc;
Adafruit_NeoPixel strip(NEO_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);

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
void boot();
void pass(int i);
void fail(int i);
void warn(int i);
void AmPmNotify();
void sevSegTest();
void testBuzzer();
void colorWipe(uint32_t color, int wait);
void alarm(unsigned int durationMs);
// End of function prototypes

void setup() {
    Serial.begin(9600);
    Serial.println("MegaClock Starting...");
    boot();
}

void loop() {
    // Main loop code here
    DateTime time = rtc.now();
    int hour = time.hour();
    int minute = time.minute();

    if (AMPM) {
        hour = hour % 12;
        if (hour == 0) hour = 12; // Handle midnight and noon
        isPM = time.isPM();
    }

    //Full Timestamp
    Serial.println(String("DateTime::TIMESTAMP_FULL:\t")+time.timestamp(DateTime::TIMESTAMP_FULL));
    Serial.println(String("Current Time: ")+String(hour)+":"+String(minute, DEC)+(isPM ? " PM" : " AM"));
    Serial.println();

    // Blink colon for PM
    display.showNumberDecEx(hour, 0b01000000, true, 2, 0); // Display hours with leading zero
    display.showNumberDec(minute, true, 2, 2); // Display minutes with leading zero
    delay(500);
    display.showNumberDecEx(hour, 0b00000000, true, 2, 0); // Display hours without colon
    display.showNumberDec(minute, true, 2, 2); // Display minutes with leading zero
    display.showNumberDec(minute, true, 2, 2); // Display minutes with leading zero

    if(hour == 7 && minute >= 15 && minute <= 20 && isPM == false) {
        alarm(10000); // Alarm for 10 seconds
        delay(10000); // Wait an additional 10 seconds to avoid multiple triggers
    }

    if(isPM) {
        digitalWrite(13, HIGH); // Turn on PM indicator LED
    } else {
        digitalWrite(13, LOW); // Turn off PM indicator LED
    }
    
    delay(500);
}

void boot() {
    Serial.println("Initializing NeoPixels...");
    if (!strip.begin()) {
        Serial.println("Neopixels Initialization FAILED!!!");
        warn(0);
    }
    strip.show();
    strip.setBrightness(10);
    strip.show();
    fail(0);
    delay(500);
    warn(0);
    delay(500);
    pass(0);
    delay(500);
    strip.clear();
    strip.setPixelColor(0, strip.Color(0, 0, 255));
    strip.show();
    Serial.println("NeoPixels Initialized.");
    pass(1);

    Serial.println("Initializing 7-Segment Display...");
    display.setBrightness(0x0f); // Set maximum blindness
    Serial.println("7-Segment Display Initialized");
    pass(2);

    Serial.println("Initializing RTC...");

    if (rtc.begin()) {
        Serial.println("RTC Ok");
        pass(3);
    } else {
        Serial.println("RTC not found.");
        fail(3);
        while(1);
    }

    if (rtc.isrunning()) {
        Serial.println("RTC is running.");
        pass(4);
    } else{
        Serial.println("RTC is NOT running, setting the time!");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        warn(4);
    }

    Serial.println("RTC Initialized.");
    //rtc.adjust(DateTime(2024, 11, 4, 21, 35, 0));

    Serial.println("Initializing Buzzer...");
    pinMode(buzzer, OUTPUT);
    digitalWrite(buzzer, LOW); // Ensure buzzer is off
    Serial.println("Testing Buzzer...");
    testBuzzer();
    Serial.println("Buzzer Initialized.");
    pass(5);

    AmPmNotify(); // Warn about time format
    warn(6);

    delay(1000);
    colorWipe(strip.Color(0, 0, 255), 20);
    colorWipe(strip.Color(0, 0, 0), 20);
    strip.setBrightness(50);
    strip.show();
}

void pass(int i) {
    strip.setPixelColor(i, strip.Color(0, 255, 0));
    strip.show();
}

void fail(int i) {
    strip.setPixelColor(i, strip.Color(255, 0, 0));
    strip.show();
}

void warn(int i) {
    strip.setPixelColor(i, strip.Color(128, 128, 0));
    strip.show();
}

void AmPmNotify() {
    if(ampm == true) {
        Serial.println("Using 12-Hour format");
    } else {
        Serial.println("Using 24-Hour format");
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

void testBuzzer() {
    for (int i = 0; i < 2; i++) {
        digitalWrite(buzzer, HIGH);
        delay(50);
        digitalWrite(buzzer, LOW);
        delay(50);
    }
}

void colorWipe(uint32_t color, int wait) {
  for(unsigned int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

void alarm(unsigned int durationMs) {
    unsigned long start = millis();
    while (millis() - start < durationMs) {
        digitalWrite(buzzer, HIGH);
        colorWipe(strip.Color(255, 0, 0), 20); // Red wipe
        digitalWrite(buzzer, LOW);
        colorWipe(strip.Color(0, 0, 0), 20);   // Off wipe
    }
}