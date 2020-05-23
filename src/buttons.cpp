#include <buttons.hpp>

void btn1Callback(voidFuncPtr cb) {
    pinMode(BUTTON_1, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_1), cb, FALLING);
}
void btn2Callback(voidFuncPtr cb) {
    pinMode(BUTTON_2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_2), cb, FALLING);
}

void btnInit() {


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