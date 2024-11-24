#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <Adafruit_NeoPixel.h>

// Enum for different light patterns
enum Pattern { SOLID, FLASH, BREATHE, WIPE, RAINBOW, CUSTOM };

// Define a function pointer type for custom pattern functions
typedef void (*CustomPatternFunc)(Adafruit_NeoPixel&);

class LightManager {
public:
    LightManager(uint8_t numLeds, uint8_t pin);
    void begin();
    void update();
    void setPattern(Pattern pattern, uint32_t color = 0, unsigned long interval = 1000);
    void setCustomPattern(CustomPatternFunc customFunc);  // Use function pointer instead of std::function
    void setBrightness(uint8_t brightness);
    void setPatternDuration(unsigned long duration);
    void pause();
    void resume();
    Adafruit_NeoPixel strip;
private:
    uint32_t wheel(byte wheelPos);
    uint32_t dimColor(uint32_t color, uint8_t brightness);
    void setSolidColor(uint32_t color); // Added declaration for setSolidColor
    uint8_t numLeds;
    uint16_t j;
    bool isOn;
    int currentLed;
    int brightness;
    int fadeAmount;
    unsigned long lastUpdate;
    unsigned long patternInterval;
    unsigned long patternDuration;
    unsigned long patternStartTime;
    Pattern currentPattern;
    uint32_t currentColor;
    CustomPatternFunc customPatternFunc;  // Use function pointer for custom pattern
    void executePattern();
    void flashPattern(uint32_t color);
    void breathePattern(uint32_t color);
    void wipePattern(uint32_t color);
    void rainbowCycle();
};

#endif
