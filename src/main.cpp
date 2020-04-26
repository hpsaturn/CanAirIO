#include <Button2.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <Wire.h>
#include <hpma115S0.h>

#include <battery.hpp>
#include <hal.hpp>

#include "WiFi.h"
// #include <vector>

#ifdef ENABLE_TFT
TFT_eSPI tft = TFT_eSPI(135, 240);  // Invoke custom library
#endif
Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

char buff[512];
long count = 0;
bool btn1click;
bool sensorToggle;

HardwareSerial hpmaSerial(1);
HPMA115S0 hpma115S0(hpmaSerial);

// vector<unsigned int> v25;      // for average
// vector<unsigned int> v10;      // for average
// unsigned int apm25 = 0;        // last PM2.5 average
// unsigned int apm10 = 0;        // last PM10 average
#define SENSOR_RETRY 1000  // Sensor read retry

//! Long time delay, it is recommended to use shallow sleep, which can effectively reduce the current consumption
void espDelay(int ms) {
    esp_sleep_enable_timer_wakeup(ms * 1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_light_sleep_start();
}

void showVoltage() {
    static uint64_t timeStamp = 0;
    if (millis() - timeStamp > 1000) {
        timeStamp = millis();
        float battery_voltage = getVoltage();
        String voltage = "" + String(battery_voltage) + "v";
        Serial.printf("-->[UI] voltage: %s\n", voltage.c_str());
#ifdef ENABLE_TFT
        tft.fillScreen(TFT_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.setTextSize(2);
        tft.drawString("Voltage:", tft.width() / 2, tft.height() / 2 - 24);
        tft.setTextSize(4);
        tft.drawString(voltage, tft.width() / 2, tft.height() / 2 + 8);
#endif
    }
}

void wifi_scan() {
    count = 0;
#ifdef ENABLE_TFT
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(2);
    tft.drawString("Scan Network", tft.width() / 2, tft.height() / 2);
#endif
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    int16_t n = WiFi.scanNetworks();

#ifdef ENABLE_TFT
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
    if (n == 0) {
        tft.drawString("no networks found", tft.width() / 2, tft.height() / 2);
    } else {
        tft.setTextDatum(TL_DATUM);
        tft.setCursor(0, 0);
        for (int i = 0; i < n; ++i) {
            sprintf(buff,
                    "[%d]:%s(%d)",
                    i + 1,
                    WiFi.SSID(i).c_str(),
                    WiFi.RSSI(i));
            tft.println(buff);
        }
    }
#endif
    WiFi.mode(WIFI_OFF);
    Serial.printf("-->[WiFi] Found %d net\n", n);
    espDelay(2000);
}

void sensorInit() {
    Serial.println("-->[PMSensor] Starting Panasonic sensor..");
    delay(100);
    hpmaSerial.begin(9600, SERIAL_8N1, PMS_RX, PMS_TX);
    delay(100);
}

void showSensorStatus() {
#ifdef ENABLE_TFT
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(2);
    tft.drawString("Enable Sensor:", tft.width() / 2, tft.height() / 2 - 24);
    tft.setTextSize(4);
    tft.drawString(String(sensorToggle), tft.width() / 2, tft.height() / 2 + 8);
#endif
    Serial.printf("-->[UI] Enable sensor: %s\n", sensorToggle ? "true" : "false");
}

void enableSensor(bool enable) {
    if (enable)
        digitalWrite(PMS_EN, HIGH);
    else
        digitalWrite(PMS_EN, LOW);
}

void wrongDataState() {
    Serial.println("-->[E][PMSensor] !wrong data!");
    hpmaSerial.end();
    sensorInit();
    delay(500);
}

char getLoaderChar() {
    char loader[] = {'/', '|', '\\', '-'};
    return loader[random(0, 4)];
}

void showValues(uint16_t pm25, uint16_t pm10) {
    char output[22];
    sprintf(output, "%03d", pm25);
    Serial.println(output);
#ifdef ENABLE_TFT
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(8);
    tft.fillScreen(TFT_BLACK);
    tft.drawString(String(output), tft.width() / 2, tft.height() / 2);
    float volts = getVoltage();
    String voltage = "" + String(volts) + "v";
    String battery = "" + String(calcPercentage(volts)) + "%";
    tft.setTextSize(3);
    tft.setTextDatum(BR_DATUM);
    tft.drawString(voltage, tft.width(), 135);
    tft.setTextDatum(BL_DATUM);
    tft.drawString(battery, 0, 135);
#endif
}

void sensorLoop() {
    static uint64_t timeStamp = 0;
    if (millis() - timeStamp > 5000) {
        timeStamp = millis();
        int try_sensor_read = 0;
        String txtMsg = "";
        while (txtMsg.length() < 32 && try_sensor_read++ < SENSOR_RETRY) {
            while (hpmaSerial.available() > 0) {
                char inChar = hpmaSerial.read();
                txtMsg += inChar;
                Serial.print("-->[PMSensor] read " + String(getLoaderChar()) + "\r");
            }
            Serial.print("-->[PMSensor] read " + String(getLoaderChar()) + "\r");
        }
        if (try_sensor_read > SENSOR_RETRY) {
            Serial.println("-->[PMSensor] read > fail!");
            Serial.println("-->[E][PMSensor] disconnected ?");
            delay(500);  // waiting for sensor..
        }

        if (txtMsg[0] == 02) {
            Serial.print("-->[PMSensor] read > done! ");
            unsigned int pm25 = txtMsg[6] * 256 + byte(txtMsg[5]);
            unsigned int pm10 = txtMsg[10] * 256 + byte(txtMsg[9]);
            if (pm25 < 1000 && pm10 < 1000) {
                showValues(pm25, pm10);
            } else
                wrongDataState();
        } else
            wrongDataState();
    }
}

void showWelcome() {
#ifdef ENABLE_TFT
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("MENU KEYS", tft.width() / 2, 0);
    tft.setTextDatum(TL_DATUM);
    tft.drawString("[Button1]", 0, 20);
    tft.drawString("Click: Sensor On", 0, 37);
    tft.drawString("Press: WifiScan", 0, 54);
    tft.drawString("[Button2]", 0, 71);
    tft.drawString("Click: Voltage", 0, 88);
    tft.drawString("Press: Suspend", 0, 105);
#endif
    Serial.println("-->[UI] Welcome");
}

void displayTurnOff() {
#ifdef ENABLE_TFT
    int r = digitalRead(TFT_BL);
    tft.fillScreen(TFT_BLACK);
    digitalWrite(TFT_BL, !r);
    tft.writecommand(TFT_DISPOFF);
    tft.writecommand(TFT_SLPIN);
#endif
}

void displayInit() {
#ifdef ENABLE_TFT
    tft.init();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN);
    tft.setCursor(0, 0);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(2);
    if (TFT_BL > 0) {                            // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
        pinMode(TFT_BL, OUTPUT);                 // Set backlight pin to output mode
        digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);  // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
    }
#endif
}

void suspend() {
    count = 0;
    enableSensor(false);
    displayTurnOff();
    //After using light sleep, you need to disable timer wake, because here use external IO port to wake up
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
    // esp_sleep_enable_ext1_wakeup(GPIO_SEL_35, ESP_EXT1_WAKEUP_ALL_LOW);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_35, 0);
    delay(200);
    esp_deep_sleep_start();
}

void buttonInit() {
    btn1.setLongClickHandler([](Button2 &b) {
        Serial.println("-->[Btn1LngPress:] Suspend..");
        btn1click = false;
        suspend();
    });

    btn1.setClickHandler([](Button2 &b) {
        Serial.println("-->[Btn1 Detect:] Voltage..");
        btn1click = !btn1click;
        showVoltage();
    });

    btn2.setLongClickHandler([](Button2 &b) {
        Serial.println("-->[Btn2 Detect:] Wifi Scan..");
        btn1click = false;
        wifi_scan();
    });

    btn2.setClickHandler([](Button2 &b) {
        btn1click = false;
        sensorToggle = !sensorToggle;
        enableSensor(sensorToggle);
        showSensorStatus();
    });
}

void button_loop() {
    btn1.loop();
    btn2.loop();
}

void setup() {
    Serial.begin(115200);
    Serial.println("\n-->[SETUP] init:");
    displayInit();
    sensorInit();
    buttonInit();
    setupBattery();
    setupADC();
    pinMode(PMS_EN, OUTPUT);
    showWelcome();
}

void loop() {
    // if (count++ > 6000000) suspend();
    if (btn1click) showVoltage();
    button_loop();
    if (sensorToggle) sensorLoop();
}
