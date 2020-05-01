#include <TFT_eSPI.h>
#include <SPI.h>
#include <Wire.h>
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
void displayOff();
void displayInit();
void guiLoop();


