#include <Arduino.h>
#include <TM1637Display.h>
#include <RTClib.h>
#include <Adafruit_NeoPixel.h>

#define AMPM true // true for 12-hour format, false for 24-hour format
#if AMPM
  #warning "Using 12-hour format"
  bool isPM;
#else
  #warning "Using 24-hour format"
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
void sevSegTest();
void testBuzzer();
void colorWipe(uint32_t color, int wait);
void alarm(unsigned int durationMs);
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
    strip.show();
    Serial.println("NeoPixel Strip Initialized.");

    Serial.println("Initializing Buzzer...");
    pinMode(buzzer, OUTPUT);
    digitalWrite(buzzer, LOW); // Ensure buzzer is off
    Serial.println("Testing Buzzer...");
    testBuzzer();
    Serial.println("Buzzer Initialized.");
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

    if(hour == 6 && minute >= 15) {
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
    Serial.println("Testing Buzzer...");
    for (int i = 0; i < 3; i++) {
        digitalWrite(buzzer, HIGH);
        delay(50);
        digitalWrite(buzzer, LOW);
        delay(50);
    }
    Serial.println("Buzzer Test Complete.");
}

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
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