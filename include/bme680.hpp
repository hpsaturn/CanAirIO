#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

#define SEALEVELPRESSURE_HPA (1013.25)

void bmeInit();
void bmeLoop();

String getTemperature();
String getPressure();
String getHumidity();
String getGas();
String getAltitude();

