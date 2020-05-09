#include <TFT_eSPI.h>
#include <battery.hpp>
#include <bme680.hpp>
#include <pmsensor.hpp>
#include <hal.hpp>

void showMainPage();
void showVoltagePage();
void showBatteryStatus();
void showBME680Values();
void showPMSValues();
void showWelcome();
void showUptime(uint32_t uptime);
void showStatus();
void showBTIcon(bool isPaired);
void showWifiIcon(bool isWifiOn);
void showDataIcon(bool isDataOn);
void displayOff();
void displayInit();
void guiLoop();


