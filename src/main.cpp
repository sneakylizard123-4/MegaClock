#include <Arduino.h>
#include <TM1637Display.h>

#define CLK_PIN 2
#define DIO_PIN 3

TM1637Display display(CLK_PIN, DIO_PIN);

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
}

void loop() {
    // Main loop code here
    display.setBrightness(0x0f); // Set maximum brightness
    display.setSegments(SEG_DONE);
    delay(1000);
    display.setSegments(SEG_TRUE);
    delay(1000);
    display.setSegments(SEG_OUCH);
    delay(1000);
}