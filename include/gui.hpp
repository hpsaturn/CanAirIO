#include <TFT_eSPI.h>
#include <SPI.h>
#include <Wire.h>
#include <battery.hpp>
#include <bme680.hpp>
#include <pmsensor.hpp>
#include <hal.hpp>

void showVoltage();
void showBME680Values();
void showPMSValues();
void showWelcome();
void displayTurnOff();
void displayInit();
void guiLoop();


