#include <Arduino.h>
#include <ConfigApp.hpp>
#include <OTAHandler.h>
#include <Wire.h>
#include <SPI.h>
#include <sleep.hpp>
#include <gui.hpp>
#include <buttons.hpp>
#include <wifi.hpp>
#include <bluetooth.hpp>
#include <hal.hpp>
#include "esp_log.h"

bool isClickedBtnOff;
bool isClickedBtnPage;

void turnOff () {
    displayOff();
    espDeepSleep();
}

void onBtn1Click() {
    isClickedBtnOff = true;
}

void onBtn2Click() {
    isClickedBtnPage = true;
}

void setup() {
    Serial.begin(115200);
    Serial.println("\n-->[SETUP] init:");
    cfg.init("canairio");     // init all preferences.

    // esp_log_level_set("*", ESP_LOG_DEBUG);     // set all components to ERROR level
    // esp_log_level_set("wifi", ESP_LOG_WARN);   // enable WARN logs from WiFi stack
    // esp_log_level_set("dhcpc", ESP_LOG_INFO);  // enable INFO logs from DHCP client

    displayInit();            // tft connection and settings.
    showMainPage();           // gui: main page (sensor values)
    bleServerInit();          // BLE GATT server init and advertising.
    pmsensorInit();           // Panasonic PM sensor initialization.
    bmeInit();                // BME680 multi sensor initialization.
    btnInit();                // Buttons interrupts.
    btn1Callback(onBtn1Click);
    btn2Callback(onBtn2Click);
    setupBattery();           // init battery ADC.
    setupBattADC();           // confit battery ADC.
    wifiInit();               // re-connection to last wifi network.
    influxDbInit();           // init influxdb connection
    apiInit();                // init API connection
}

void loop() {
    bmeLoop();                      // the BME680 sensor loop.
    pmsensorLoop(bleIsConnected()); // if phone is connected, the turnon
                                    // sensor interval is minor.
    wifiLoop();                     // check wifi or reconnect it.
    bleLoop();                      // notify data to phone or BLE device
    apiLoop();                      // CanAir.io API publication.
    influxDbLoop();                 // influxDB publication.
    otaLoop();                      // check for firmware updates.
    guiLoop();                      // gui ttf refresh methods.

    // save battery after phone disconnected:
    // if (!bleIsConnected()) espShallowSleep(5000);  
    if (isClickedBtnOff) turnOff();
}
