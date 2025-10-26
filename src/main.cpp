#include <Arduino.h>
#include <TM1637Display.h>
#include <RTClib.h>

#define CLK_PIN 2
#define DIO_PIN 3

TM1637Display display(CLK_PIN, DIO_PIN);
RTC_DS1307 rtc;

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
    //rtc.adjust(DateTime(2024, 10, 26, 11, 49, 15));
}

void loop() {
    // Main loop code here
    DateTime time = rtc.now();

    //Full Timestamp
    Serial.println(String("DateTime::TIMESTAMP_FULL:\t")+time.timestamp(DateTime::TIMESTAMP_FULL));
    //Date Only
    Serial.println(String("DateTime::TIMESTAMP_DATE:\t")+time.timestamp(DateTime::TIMESTAMP_DATE));
    //Full Timestamp
    Serial.println(String("DateTime::TIMESTAMP_TIME:\t")+time.timestamp(DateTime::TIMESTAMP_TIME));
    Serial.println();

    display.setBrightness(0x0f); // Set maximum brightness
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