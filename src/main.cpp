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

char buff[512];
long count = 0;
bool lowPowerMode;

void setup() {
    Serial.begin(115200);
    Serial.println("\n-->[SETUP] init:");
    cfg.init("canairio");     // init all preferences.
    displayInit();            // tft connection and settings.
    showMainPage();           // gui: main page (sensor values)
    bleServerInit();          // BLE GATT server init and advertising.
    pmsensorInit();           // Panasonic PM sensor initialization.
    bmeInit();                // BME680 multi sensor initialization.
    btnInit();                // Buttons interrupts.
    setupBattery();           // init battery ADC.
    setupBattADC();           // confit battery ADC.
    wifiInit();               // re-connection to last wifi network.
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
    if (!bleIsConnected()) espShallowSleep(5000);  
}
