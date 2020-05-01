#include <Button2.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <Wire.h>
#include <battery.hpp>
#include <bme680.hpp>
#include <pmsensor.hpp>
#include <sleep.hpp>
#include <hal.hpp>

#include "WiFi.h"
// #include <vector>

#ifdef ENABLE_TFT
TFT_eSPI tft = TFT_eSPI(135, 240);  // Invoke custom library
#endif

// Button2 btn1(BUTTON_1);
// Button2 btn2(BUTTON_2);

char buff[512];
long count = 0;
bool btn1click;
bool lowPowerMode;

void showVoltage() {
    static uint64_t timeStamp = 0;
    if (millis() - timeStamp > 3000) {
        timeStamp = millis();
        float volts = battGetVoltage();
        String voltage = "" + String(volts) + "v";
        Serial.printf("-->[UI] voltage: %s\n", voltage.c_str());
#ifdef ENABLE_TFT
        tft.fillScreen(TFT_BLACK);
        tft.setTextDatum(TC_DATUM);
        tft.setTextSize(2);
        tft.drawString("Voltage:", tft.width()/2 , 0);
        tft.setTextDatum(MC_DATUM);
        tft.setTextSize(6);
        tft.drawString(voltage, tft.width() / 2, tft.height() / 2 );
        String percentage = "" + String(battCalcPercentage(volts)) + "%";
        tft.setTextSize(3);
        tft.drawString(percentage, tft.width() / 2, tft.height() / 2 + 44);
#endif
    }
}

void wifi_scan() {
    count = 0;
#ifdef ENABLE_TFT
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(2);
    tft.drawString("Scan Network", tft.width() / 2, tft.height() / 2);
#endif
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    int16_t n = WiFi.scanNetworks();

#ifdef ENABLE_TFT
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
    if (n == 0) {
        tft.drawString("no networks found", tft.width() / 2, tft.height() / 2);
    } else {
        tft.setTextDatum(TL_DATUM);
        tft.setCursor(0, 0);
        for (int i = 0; i < n; ++i) {
            sprintf(buff,
                    "[%d]:%s(%d)",
                    i + 1,
                    WiFi.SSID(i).c_str(),
                    WiFi.RSSI(i));
            tft.println(buff);
        }
    }
#endif
    WiFi.mode(WIFI_OFF);
    Serial.printf("-->[WiFi] Found %d net\n", n);
    espShallowSleep(2000);
}

void showBME680Values() {
    // static uint64_t timeStamp = 0;
    // if (millis() - timeStamp > 5000) {
    //     timeStamp = millis();
#ifdef ENABLE_TFT
        tft.setTextSize(2);
        tft.setTextDatum(BL_DATUM);
        String outln1 = "T:" + getTemperature() + " HR:" + getHumidity();
        tft.drawString(outln1, 0, tft.height() - 36);
        String outln2 = "P:" + getPressure() + " A:" + getAltitude();
        tft.drawString(outln2, 0, tft.height() - 18);
        String outln3 = "GAS:" + getGas();
        tft.drawString(outln3, 0, tft.height());
#endif
    // }
}

void showPMSValues() {
    // static uint64_t timeStamp = 0;
    // if (millis() - timeStamp > 5000) {
    //     timeStamp = millis();
#ifdef ENABLE_TFT
        tft.fillScreen(TFT_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.setTextSize(8);
        tft.drawString(getStringPM25(), tft.width() / 2, tft.height() / 2 - 24);
        tft.setTextSize(1);
        tft.drawString("PM2.5", tft.width() / 2 + 75, tft.height() / 2 - 2);
        float volts = battGetVoltage();
        String voltage = "" + String(volts) + "v";
        String battery = "BATT:" + String(battCalcPercentage(volts)) + "%";
        tft.setTextSize(1);
        tft.drawString(battery, 0, 0);
        tft.setTextDatum(TR_DATUM);
        tft.drawString(voltage, tft.width(), 0);
#endif
    // }
}

void showWelcome() {
#ifdef ENABLE_TFT
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("MENU KEYS", tft.width() / 2, 0);
    tft.setTextDatum(TL_DATUM);
    tft.drawString("[Button1]", 0, 20);
    tft.drawString("Click: Sensor On", 0, 37);
    tft.drawString("Press: WifiScan", 0, 54);
    tft.drawString("[Button2]", 0, 71);
    tft.drawString("Click: Voltage", 0, 88);
    tft.drawString("Press: Suspend", 0, 105);
#endif
    Serial.println("-->[UI] Welcome");
}

void displayTurnOff() {
#ifdef ENABLE_TFT
    int r = digitalRead(TFT_BL);
    tft.fillScreen(TFT_BLACK);
    digitalWrite(TFT_BL, !r);
    tft.writecommand(TFT_DISPOFF);
    tft.writecommand(TFT_SLPIN);
#endif
}

void displayInit() {
#ifdef ENABLE_TFT
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN);
    tft.setCursor(0, 0);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(2);
    if (TFT_BL > 0) {                            // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
        pinMode(TFT_BL, OUTPUT);                 // Set backlight pin to output mode
        digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);  // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
    }
#endif
}

void btn1Click() {
    Serial.println("-->[Btn1LngPress:] Suspend..");
    displayTurnOff();
    espDeepSleep();
}

void btn2Click() {
    Serial.println("-->[Btn2 Detect:] Voltage..");
    showVoltage();
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
    showPMSValues();
    showBME680Values();
    pmsensorInit();
    buttonInit();
    setupBattery();
    setupBattADC();
    bmeInit();
}

void loop() {
    bmeLoop();
    pmsensorLoop();
    showPMSValues();
    showBME680Values();
    espShallowSleep(5000);
}
