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
    displayInit();
    showMainPage();
    cfg.init("canairio");
    pmsensorInit();
    btnInit();
    setupBattery();
    setupBattADC();
    bmeInit();
    bleServerInit();
    wifiInit();
    influxDbInit();
    apiInit();
}

void loop() {

    bmeLoop();
    pmsensorLoop();
    guiLoop();

    bleLoop();       // notify data to connected devices
    wifiLoop();      // check wifi and reconnect it
    apiLoop();       // CanAir.io API publication
    influxDbLoop();  // influxDB publication
    // statusLoop();    // update sensor status GUI
    otaLoop();       // check for firmware updates
    // delay(500);
    // espShallowSleep(1000);
}
