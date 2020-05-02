#include "bme680.hpp"

Adafruit_BME680 bme; // I2C
float altitude;

void bmeInit() {
    if (!bme.begin()) {
        Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
        return;
    }
    // Set up oversampling and filter initialization
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme.setGasHeater(320, 150);  // 320*C for 150 ms
}

void bmeLoop() {
    static uint64_t timeStamp = 0;
    if (millis() - timeStamp > 5000) {
        timeStamp = millis();
        // Tell BME680 to begin measurement.
        unsigned long endTime = bme.beginReading();
        if (endTime == 0) {
            Serial.println(F("-->[BME680] Failed to begin reading :("));
            return;
        }
        Serial.print(F("-->[BME680] Reading started at "));
        Serial.print(millis());
        Serial.print(F(" and will finish at "));
        Serial.println(endTime);
        // There's no need to delay() until millis() >= endTime: bme.endReading()
        // takes care of that. It's okay for parallel work to take longer than
        // BME680's measurement time.

        // Obtain measurement results from BME680. Note that this operation isn't
        // instantaneous even if milli() >= endTime due to I2C/SPI latency.
        if (!bme.endReading()) {
            Serial.println(F("-->[BME680] Failed to complete reading :("));
            return;
        }
        Serial.print(F("-->[BME680] Reading completed at "));
        Serial.println(millis());

        Serial.print(F("-->[BME680] T:"));
        Serial.print(bme.temperature);
        Serial.print(F("C "));

        Serial.print(F("P:"));
        Serial.print(bme.pressure / 100.0);
        Serial.print(F("hPa "));

        Serial.print(F("H:"));
        Serial.print(bme.humidity);
        Serial.print(F("% "));

        Serial.print(F("G:"));
        Serial.print(bme.gas_resistance / 1000.0);
        Serial.print(F("KOhms "));

        Serial.print(F("A:"));
        altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
        Serial.print(altitude);
        Serial.println(F("m"));
    }
}

float getTemperature(){
  return bme.temperature;
}

float getHumidity(){
  return bme.humidity;
}

float getPressure(){
  return bme.pressure / 100.0;
}

float getAltitude(){
  return altitude;
}

float getGas(){
  return bme.gas_resistance / 1000.0;
}

String getFormatTemp() {
    return String(bme.temperature) + "C";
}

String getFormatPress() {
    return String(bme.pressure / 100.0) + "hPa";
}

String getFormatHum() {
    return String(bme.humidity) + "%";
}

String getFormatGas() {
    return String(bme.gas_resistance / 1000.0) + "KOhms";
}

String getFormatAlt() {
    return String((int)altitude) + "m";
}