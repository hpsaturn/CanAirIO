#include <buttons.hpp>

// Button2 btn1(BUTTON_1);
// Button2 btn2(BUTTON_2);

void btnActionDeepSleep() {
    Serial.println("-->[btnActionDeepSleep]");
    displayOff();
    espDeepSleep();
}

void btnActionVoltagePage() {
    Serial.println("-->[btnActionVoltagePage]");
    showVoltagePage();
}

void btnInit() {

    pinMode(BUTTON_2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_2), btnActionDeepSleep, FALLING);

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