#include "pmsensor.hpp"

// vector<unsigned int> v25;      // for average
// vector<unsigned int> v10;      // for average
// unsigned int apm25 = 0;        // last PM2.5 average
// unsigned int apm10 = 0;        // last PM10 average

HardwareSerial hpmaSerial(1);
HPMA115S0 hpma115S0(hpmaSerial);

uint16_t pm25;
uint16_t pm10;
int sampleCount = 0;
bool isInitSetup = true;
int initSetupCount = 0;

void pmsensorInit() {
    Serial.println("-->[PMSensor] Setup Panasonic PM sensor..");
    delay(100);
    hpmaSerial.begin(9600, SERIAL_8N1, PMS_RX, PMS_TX);
    pinMode(PMS_EN, OUTPUT);
    delay(100);
}

void pmsensorEnable(bool enable) {
    if (enable)
        digitalWrite(PMS_EN, HIGH);
    else
        digitalWrite(PMS_EN, LOW);
}

void pmsensorRead() {
    int try_sensor_read = 0;
    String txtMsg = "";
    while (txtMsg.length() < 32 && try_sensor_read++ < SENSOR_RETRY) {
        while (hpmaSerial.available() > 0) {
            char inChar = hpmaSerial.read();
            txtMsg += inChar;
            Serial.print("-->[PMSensor] read " + String(_getLoaderChar()) + "\r");
        }
        Serial.print("-->[PMSensor] read " + String(_getLoaderChar()) + "\r");
    }
    if (try_sensor_read > SENSOR_RETRY) {
        Serial.println("-->[PMSensor] read > fail!");
        Serial.println("-->[E][PMSensor] disconnected ?");
        delay(500);  // waiting for sensor..
    }
    if (txtMsg[0] == 02) {
        pm25 = txtMsg[6] * 256 + byte(txtMsg[5]);
        pm10 = txtMsg[10] * 256 + byte(txtMsg[9]);
        Serial.print("-->[PMSensor] read > done! ");
        Serial.printf("==> [#%02d] PM25:%03d PM10:%03d\n",++sampleCount, getPM25(), getPM10());
    } else
        _wrongDataState();
}

void pmsensorLoop() {
    static uint64_t pmTimeStamp = 0;
    if ((millis() - pmTimeStamp > SENSOR_INTERVAL)) {
        pmsensorEnable(true);
        if ((millis() - pmTimeStamp) > (SENSOR_INTERVAL + SENSOR_SAMPLE)) {
            pmsensorRead(); 
            pmTimeStamp = millis();
            pmsensorEnable(false);
            sampleCount = 0;
            Serial.println("-->[PMSensor] disable.");
        } else if ((millis() - pmTimeStamp > SENSOR_INTERVAL + SENSOR_SAMPLE / 2)) {
            pmsensorRead(); 
        } else {
            Serial.println("-->[PMSensor] waiting for stable measure..");
        }
    } else if (isInitSetup) {
        pmsensorEnable(true);
        pmsensorRead();
        if (initSetupCount++ > 4) {
            isInitSetup = false;
            sampleCount = 0;
            pmsensorEnable(false);
            Serial.println("-->[PMSensor] Setup done.");
        }
    } 
}

uint16_t getPM25() {
    return pm25;
}

String getStringPM25() {
    char output[5];
    sprintf(output, "%03d", getPM25());
    return String(output);
}

uint16_t getPM10() {
    return pm10;
}

String getStringPM10() {
    char output[5];
    sprintf(output, "%03d", getPM10());
    return String(output);
}

void _wrongDataState() {
    Serial.println("-->[E][PMSensor] !wrong data!");
    hpmaSerial.end();
    pmsensorInit();
    delay(100);
}

char _getLoaderChar() {
    char loader[] = {'/', '|', '\\', '-'};
    return loader[random(0, 4)];
}