#include "LightManager.h"

LightManager::LightManager(uint8_t numLeds, uint8_t pin)
    : strip(numLeds, pin, NEO_GRB + NEO_KHZ800), numLeds(numLeds), j(0), isOn(false), currentLed(0), brightness(255), fadeAmount(5), lastUpdate(0), patternInterval(1000), patternDuration(0), patternStartTime(0), currentPattern(SOLID), currentColor(0), customPatternFunc(nullptr) {}

void LightManager::begin() {
    strip.begin();
    strip.show();
}

void LightManager::update() {
    if (patternDuration > 0 && millis() - patternStartTime >= patternDuration) {
        return; // Stop updating if duration is exceeded
    }
    executePattern();
}

void LightManager::setPattern(Pattern pattern, uint32_t color, unsigned long interval) {
    currentPattern = pattern;
    currentColor = color;
    patternInterval = interval;
    patternStartTime = millis();
}

void LightManager::setCustomPattern(CustomPatternFunc customFunc) {
    customPatternFunc = customFunc;
    currentPattern = CUSTOM;
}

void LightManager::setBrightness(uint8_t brightness) {
    this->brightness = brightness;
    strip.setBrightness(brightness);
}

void LightManager::setPatternDuration(unsigned long duration) {
    patternDuration = duration;
}

void LightManager::pause() {
    patternInterval = 0; // Stop updating the pattern
}

void LightManager::resume() {
    patternInterval = 1000; // Reset to default or previous interval
}

void LightManager::executePattern() {
    if (millis() - lastUpdate >= patternInterval) {
        lastUpdate = millis();
        switch (currentPattern) {
            case SOLID: setSolidColor(currentColor); break;
            case FLASH: flashPattern(currentColor); break;
            case BREATHE: breathePattern(currentColor); break;
            case WIPE: wipePattern(currentColor); break;
            case RAINBOW: rainbowCycle(); break;
            case CUSTOM: if (customPatternFunc) customPatternFunc(strip); break;
        }
        strip.show();
    }
}

void LightManager::setSolidColor(uint32_t color) {
    for (int i = 0; i < numLeds; i++) {
        strip.setPixelColor(i, color);
    }
}

void LightManager::flashPattern(uint32_t color) {
    isOn = !isOn;
    setSolidColor(isOn ? color : strip.Color(0, 0, 0));
}

void LightManager::breathePattern(uint32_t color) {
    brightness += fadeAmount;
    if (brightness <= 0 || brightness >= 255) {
        fadeAmount = -fadeAmount; // Reverse the fade direction
    }
    for (int i = 0; i < numLeds; i++) {
        strip.setPixelColor(i, dimColor(color, brightness));
    }
}

void LightManager::wipePattern(uint32_t color) {
    strip.setPixelColor(currentLed, color);
    currentLed = (currentLed + 1) % numLeds; // Cycle through LEDs
}

void LightManager::rainbowCycle() {
    for (int i = 0; i < numLeds; i++) {
        strip.setPixelColor(i, wheel(((i * 256 / numLeds) + j) & 255));
    }
    j = (j + 1) % 256; // Increment the wheel position
}

uint32_t LightManager::dimColor(uint32_t color, uint8_t brightness) {
    uint8_t r = (uint8_t)(color >> 16);
    uint8_t g = (uint8_t)(color >> 8);
    uint8_t b = (uint8_t)color;
    r = (r * brightness) >> 8;
    g = (g * brightness) >> 8;
    b = (b * brightness) >> 8;
    return strip.Color(r, g, b);
}

uint32_t LightManager::wheel(byte wheelPos) {
    wheelPos = 255 - wheelPos;
    if (wheelPos < 85) {
        return strip.Color(255 - wheelPos * 3, 0, wheelPos * 3);
    }
    if (wheelPos < 170) {
        wheelPos -= 85;
        return strip.Color(0, wheelPos * 3, 255 - wheelPos * 3);
    }
    wheelPos -= 170;
    return strip.Color(wheelPos * 3, 255 - wheelPos * 3, 0);
}
