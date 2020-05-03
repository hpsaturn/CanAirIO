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
    cfg.init("canairio");
    displayInit();
    showMainPage();
    wifiInit();
    pmsensorInit();
    btnInit();
    setupBattery();
    setupBattADC();
    bmeInit();
    bleServerInit();
    influxDbInit();
    apiInit();
}

void loop() {
    bmeLoop();                      // BME680 sensor loop
    pmsensorLoop(bleIsConnected()); // PM Sensor, if phone is connected
                                    // the capture interval is minor
    guiLoop();

    bleLoop();       // notify data to phone BLE device
    wifiLoop();      // check wifi or reconnect
    apiLoop();       // CanAir.io API publication
    influxDbLoop();  // influxDB publication
    otaLoop();       // check for firmware updates
    // statusLoop();    // update sensor status GUI

    if (!bleIsConnected()) espShallowSleep(5000);  // save battery
}
