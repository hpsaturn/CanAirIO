#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <sleep.hpp>
#include <gui.hpp>
#include <buttons.hpp>
#include <hal.hpp>
#include "WiFi.h"

char buff[512];
long count = 0;
bool lowPowerMode;

void setup() {
    Serial.begin(115200);
    Serial.println("\n-->[SETUP] init:");
    displayInit();
    showMainPage();
    pmsensorInit();
    btnInit();
    setupBattery();
    setupBattADC();
    bmeInit();
}

void loop() {
    bmeLoop();
    pmsensorLoop();
    guiLoop();
    espShallowSleep(5000);
}
