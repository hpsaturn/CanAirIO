#include "pmsensor.hpp"

HardwareSerial hpmaSerial(1);
HPMA115S0 hpma115S0(hpmaSerial);

uint16_t pm1;
uint16_t pm25;
uint16_t pm10;

uint16_t tpm1;
uint16_t tpm25;
uint16_t tpm10;

int scount = 0;
bool isInitSetup = true;
int initSetupCount = 0;
bool dataReady = false;
bool isPmsensorEnable=false;

void pmsensorInit() {
    Serial.println("-->[PMSensor] Setup Panasonic PM sensor..");
    hpmaSerial.begin(9600, SERIAL_8N1, PMS_RX, PMS_TX);
    pinMode(PMS_EN, OUTPUT);
    delay(100);
}

void pmsensorEnable(bool enable) {
    if (enable)
        digitalWrite(PMS_EN, HIGH);
    else
        digitalWrite(PMS_EN, LOW);
    isPmsensorEnable=enable;
}

void copyLastVars() {
    pm1 = tpm1;
    pm10 = tpm10;
    pm25 = tpm25;
    Serial.print("-->[PMSensor] Saved data: ");
    Serial.printf("[PM1:%03d][PM2.5:%03d][PM10:%03d]\n", pm1, pm25, pm10);
}

void _wrongDataState() {
    Serial.println("-->[E][PMSensor] !wrong data!");
}

char _getLoaderChar() {
    char loader[] = {'/', '|', '\\', '-'};
    return loader[random(0, 4)];
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
        tpm1 = txtMsg[2] * 256 + byte(txtMsg[1]);
        tpm25 = txtMsg[6] * 256 + byte(txtMsg[5]);
        tpm10 = txtMsg[10] * 256 + byte(txtMsg[9]);
        Serial.print("-->[PMSensor]");
        Serial.printf("[S%02d][PM1:%03d][PM2.5:%03d][PM10:%03d]\n", ++scount, tpm1, tpm25, tpm10);
    } else
        _wrongDataState();
}

void pmsensorLoop(bool isBleConnected) {
    static uint64_t pmLoopTimeStamp = 0;            // timestamp for loop check
    if ((millis() - pmLoopTimeStamp > 1000)) {
        pmLoopTimeStamp = millis();
        static uint64_t pmTimeStamp = 0;            // timestamp for interval and sensor sample time
        int turnon_interval = SENSOR_INTERVAL;
        if (isBleConnected) turnon_interval = turnon_interval / 3; 
        if ((millis() - pmTimeStamp > turnon_interval)) {
            pmsensorEnable(true);
            if ((millis() - pmTimeStamp) > (turnon_interval + SENSOR_SAMPLE)) {
                pmsensorRead();
                pmsensorEnable(false);
                copyLastVars();
                dataReady = true;
                pmTimeStamp = millis();
                scount = 0;
                Serial.println("-->[PMSensor] Disabled.");
            } else if ((millis() - pmTimeStamp > turnon_interval + SENSOR_SAMPLE / 2)) {
                pmsensorRead();
            } else {
                Serial.println("-->[PMSensor] Waiting for stable measure..");
            }
        } else if (isInitSetup && (millis() - pmTimeStamp > 5000)) {
            pmsensorEnable(true);
            pmsensorRead();
            pmTimeStamp = millis();
            if (initSetupCount++ > 4) {
                isInitSetup = false;
                scount = 0;
                pmsensorEnable(false);
                copyLastVars();
                Serial.println("-->[PMSensor] Setup done.");
            }
        }
    }
}

bool pmsensorDataReady() {
    return dataReady;
}

uint16_t getPM1() {
    return pm1;
}

String getStringPM1() {
    char output[5];
    sprintf(output, "%03d", getPM1());
    return String(output);
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

bool pmsensorIsEnable() {
    return isPmsensorEnable;
}
