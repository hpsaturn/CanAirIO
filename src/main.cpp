#include <Button2.h>
#include <sleep.hpp>
#include <gui.hpp>
#include <hal.hpp>

#include "WiFi.h"
// #include <vector>

// Button2 btn1(BUTTON_1);
// Button2 btn2(BUTTON_2);

char buff[512];
long count = 0;
bool btn1click;
bool lowPowerMode;

void btn1Click() {
    Serial.println("-->[Btn1LngPress:] Suspend..");
    displayOff();
    espDeepSleep();
}

void btn2Click() {
    Serial.println("-->[Btn2 Detect:] Voltage..");
    showVoltagePage();
}

// void buttonLoop() {
//     btn1.loop();
//     btn2.loop();
// }

void buttonInit() {

    pinMode(BUTTON_2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_2), btn1Click, FALLING);

    // btn1.setLongClickHandler([](Button2 &b) {
    //     btn1Click();
    // });

    // btn1.setClickHandler([](Button2 &b) {
    // });

    // btn2.setLongClickHandler([](Button2 &b) {
    //     Serial.println("-->[Btn2 Detect:] Wifi Scan..");
    //     wifi_scan();
    // });

    // btn2.setClickHandler([](Button2 &b) {
    //     showVoltage();
    // });
}

void setup() {
    Serial.begin(115200);
    Serial.println("\n-->[SETUP] init:");
    displayInit();
    showMainPage();
    pmsensorInit();
    buttonInit();
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
