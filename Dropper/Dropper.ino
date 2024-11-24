#include <Adafruit_NeoPixel.h>
#include <Servo.h>
#include "simpleHC12.h"
#include "LightManager.h"

#define LED_PIN 5
#define NUM_LEDS 8
#define SERVO_PIN 7
#define HC12_TX 16 //16
#define HC12_RX 10 //10
#define HC12_SET_PIN 14 //14
#define DROP_COMMAND "DROP"
#define BUTTON_PIN 2
LightManager lightManager(NUM_LEDS, LED_PIN);
Servo dropperServo;

simpleHC12 hc12(HC12_TX, HC12_RX, HC12_SET_PIN, 1200, 16, false); // Set baud rate to 1200 for better range

// Constants for commands and states
enum DropperState { OPEN, CLOSED };

DropperState dropperState = CLOSED;

void setup() {
    Serial.begin(9600);
    Serial.println("Setup started.");
    pinMode(SERVO_PIN, OUTPUT); 
    // Set HC-12 SET pin to output and ensure it's HIGH before beginning
    pinMode(HC12_SET_PIN, OUTPUT);
    digitalWrite(HC12_SET_PIN, HIGH);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    delay(100);  // Give module time to stabilize
    
    hc12.begin();
    
    // First try to reset to default settings
    Serial.println("Attempting to reset HC-12 to defaults...");
    //hc12.bruteSetDefault();
    //delay(1000);  // Give module time to reset
    
    // Now try to detect baud rate
    //hc12.baudDetector();
    //delay(500);
    
    // Configure module settings
    hc12.cmd("AT+B1200");    // Set desired baud rate
    delay(200);
    hc12.cmd("AT+P8");       // Set to high power mode
    delay(200);
    hc12.cmd("AT+C050");     // Set channel
    delay(200);
    hc12.cmd("AT+FU3");      // Set transmission mode
    delay(200);
    
    Serial.println("HC-12 configuration complete.");
    lightManager.begin();
    //delay(1000);
    lightManager.setPattern(SOLID, lightManager.strip.Color(255, 0, 255));
    dropperServo.attach(SERVO_PIN);
    
}

void loop() {
    static unsigned long lastPressTime = 0; // To track the last button press time
    const unsigned long debounceDelay = 200; // Debounce delay in milliseconds
    // Check for commands from Serial
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n'); // Read command from Serial
        command.trim(); // Remove any whitespace
        Serial.print("Received command from Serial: ");
        Serial.println(command);
        
        if (command == DROP_COMMAND) {
            Serial.println("DROP command received from Serial.");
            toggleDropper();
        }
    }

    if (digitalRead(BUTTON_PIN) == LOW) { // Button pressed
      unsigned long currentTime = millis();
      if (currentTime - lastPressTime > debounceDelay) { // Check for debounce
          Serial.println("Button pressed. Sending DROP command.");
          toggleDropper();
          lastPressTime = currentTime; // Update last press time
      }
    }
    // Check for commands from HC-12
    hc12.read(); // Read data from HC12
    if (hc12.dataIsReady()) {
        char* commandHC12 = hc12.getRcvData();
        String commandHC12Str(commandHC12);
        commandHC12Str.trim(); // Remove any whitespace from the string
        Serial.print("Received: ");
        Serial.println(commandHC12Str);
        if (commandHC12Str == DROP_COMMAND) {
            Serial.println("DROP command received from HC-12.");
            toggleDropper();
        }
        hc12.setReadyToReceive();
    }
  
    lightManager.update(); // Update the light manager to handle patterns
}

void toggleDropper() {
    if (dropperState == OPEN) {
        closeDropper();
    } else {
        openDropper();
    }
}

void openDropper() {
    // Open the dropper
    Serial.println("Opening the dropper.");
    lightManager.setPattern(SOLID, lightManager.strip.Color(0, 0, 0));
    delay(300);
    dropperServo.write(100); // Move to open position
    delay(300);
    lightManager.setPattern(SOLID, lightManager.strip.Color(0, 255, 0)); // Set light pattern for open state (green)
    delay(500);
    dropperState = OPEN;
}

void closeDropper() {
    // Close the dropper
    Serial.println("Closing the dropper.");
    lightManager.setPattern(SOLID, lightManager.strip.Color(0, 0, 0));
    delay(300);
    dropperServo.write(0); // Move to closed position
    delay(300);
    lightManager.setPattern(SOLID, lightManager.strip.Color(0, 0, 255)); // Set light pattern for closed state (blue)
    dropperState = CLOSED;
}
