#include <Adafruit_NeoPixel.h>
#include "simpleHC12.h"

#define LED_PIN 5
#define BUTTON_PIN 6
#define HC12_TX 3
#define HC12_RX 2
#define HC12_SET_PIN 4

Adafruit_NeoPixel led(1, LED_PIN, NEO_GRB + NEO_KHZ800);
simpleHC12 hc12(HC12_TX, HC12_RX, HC12_SET_PIN, 1200, 16, true); // Set baud rate to 1200 for better range

void setup() {
    Serial.begin(9600); // Initialize Serial communication for debugging
    Serial.println("Setup started.");
    
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    led.begin();
    hc12.safeSetBaudRate(); // Set baud rate for HC12
    hc12.cmd("AT+P8"); // Set to high power mode
    Serial.println("HC12 set to high power mode.");
    // change channel; use same for sender and receiver
    hc12.cmd("AT+C050");  

    // standard transmission mode; use same for sender and receiver
    hc12.cmd("AT+FU3");
    led.setPixelColor(0, led.Color(0, 0, 255)); // Initial color to indicate ready
    led.show();
    Serial.println("Setup completed. Ready to send commands.");
    delay(2000);
}

void loop() {
    static unsigned long lastPressTime = 0; // To track the last button press time
    const unsigned long debounceDelay = 200; // Debounce delay in milliseconds

    // Check if the button is pressed
    if (digitalRead(BUTTON_PIN) == LOW) { // Button pressed
        unsigned long currentTime = millis();
        if (currentTime - lastPressTime > debounceDelay) { // Check for debounce
            Serial.println("Button pressed. Sending DROP command.");
            sendDropCommand();
            lastPressTime = currentTime; // Update last press time
        }
    }
}

void sendDropCommand() {
    if (hc12.isReadyToSend()) {
        Serial.println("Sending DROP command via HC12.");
        hc12.print("DROP"); // Send the DROP command
        led.setPixelColor(0, led.Color(0, 255, 0)); // Green color to indicate command sent
        led.show();
        delay(500);
        led.setPixelColor(0, led.Color(0, 0, 0)); // Turn off LED
        led.show();
        Serial.println("DROP command sent. LED turned off.");
    } else {
        Serial.println("HC-12 not ready to send.");
    }
}