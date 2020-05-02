#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

#define SEALEVELPRESSURE_HPA (1013.25)

void bmeInit();
void bmeLoop();

float getTemperature();
float getHumidity();
float getPressure();
float getAltitude();
float getGas();

String getFormatTemp();
String getFormatPress();
String getFormatHum();
String getFormatGas();
String getFormatAlt();

