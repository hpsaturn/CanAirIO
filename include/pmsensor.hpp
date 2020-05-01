#include <hpma115S0.h>

#include "hal.hpp"

#define SENSOR_INTERVAL 1000 * 60 * 3  // 2 minutes => more is better for the battery
#define SENSOR_SAMPLE 1000 * 30        // 35 seconds => less is better for the battery
#define SENSOR_RETRY 1000              // Sensor read retry

void pmsensorInit();
void pmsensorEnable(bool enable);
void pmsensorRead();
void pmsensorLoop();

String getStringPM10();
String getStringPM25();
uint16_t getPM25();
uint16_t getPM10();

void _wrongDataState();
char _getLoaderChar();