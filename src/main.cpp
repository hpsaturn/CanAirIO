#include <hpma115S0.h>
#include <Button2.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <Wire.h>
// #include <vector>
#include "WiFi.h"
#include "esp_adc_cal.h"

#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN 0x10
#endif

#define ADC_EN 14
#define ADC_PIN 34
#define BUTTON_1 35
#define BUTTON_2 0

TFT_eSPI tft = TFT_eSPI(135, 240);  // Invoke custom library
Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

char buff[512];
int vref = 1100;
long count = 0;
bool btn1click;

#define HPMA_RX 12  // config for D1MIN1 board
#define HPMA_TX 13

HardwareSerial hpmaSerial(1);
HPMA115S0 hpma115S0(hpmaSerial);

// vector<unsigned int> v25;      // for average
// vector<unsigned int> v10;      // for average
// unsigned int apm25 = 0;        // last PM2.5 average
// unsigned int apm10 = 0;        // last PM10 average
#define SENSOR_RETRY  1000     // Sensor read retry

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
        uint16_t v = analogRead(ADC_PIN);
        float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
        String voltage = "" + String(battery_voltage) + "v";
        Serial.println(voltage);
        tft.fillScreen(TFT_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.setTextSize(2);
        tft.drawString("Voltage:", tft.width() / 2, tft.height() / 2 - 24);
        tft.setTextSize(4);
        tft.drawString(voltage, tft.width() / 2, tft.height() / 2 + 8);
    }
}

void wifi_scan() {
    count = 0;
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(2);

    tft.drawString("Scan Network", tft.width() / 2, tft.height() / 2);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    int16_t n = WiFi.scanNetworks();
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
    if (n == 0) {
        tft.drawString("no networks found", tft.width() / 2, tft.height() / 2);
    } else {
        tft.setTextDatum(TL_DATUM);
        tft.setCursor(0, 0);
        Serial.printf("Found %d net\n", n);
        for (int i = 0; i < n; ++i) {
            sprintf(buff,
                    "[%d]:%s(%d)",
                    i + 1,
                    WiFi.SSID(i).c_str(),
                    WiFi.RSSI(i));
            tft.println(buff);
        }
    }
    WiFi.mode(WIFI_OFF);
}

void suspend() {
    count = 0;
    int r = digitalRead(TFT_BL);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(1);
    tft.drawString("Press again to wake up", tft.width() / 2, tft.height() / 2);
    espDelay(2000);
    digitalWrite(TFT_BL, !r);

    tft.writecommand(TFT_DISPOFF);
    tft.writecommand(TFT_SLPIN);
    //After using light sleep, you need to disable timer wake, because here use external IO port to wake up
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
    // esp_sleep_enable_ext1_wakeup(GPIO_SEL_35, ESP_EXT1_WAKEUP_ALL_LOW);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_35, 0);
    delay(200);
    esp_deep_sleep_start();
}

void button_init() {
    btn1.setLongClickHandler([](Button2 &b) {
        Serial.println("Btn1LngPress: Suspend..");
        btn1click = false;
        suspend();
    });
    btn1.setPressedHandler([](Button2 &b) {
        Serial.println("Btn1 Detect Voltage..");
        btn1click = true;
        showVoltage();
    });

    btn2.setPressedHandler([](Button2 &b) {
        Serial.println("Btn2 Wifi scan");
        btn1click = false;
        wifi_scan();
    });
}

void button_loop() {
    btn1.loop();
    btn2.loop();
}

void setup() {
    Serial.begin(115200);
    Serial.println("Start");

    /*
    ADC_EN is the ADC detection enable port
    If the USB port is used for power supply, it is turned on by default.
    If it is powered by battery, it needs to be set to high level
    */
    pinMode(ADC_EN, OUTPUT);
    digitalWrite(ADC_EN, HIGH);

    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    // tft.setTextSize(2);
    tft.setTextColor(TFT_GREEN);
    tft.setCursor(0, 0);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(2);

    if (TFT_BL > 0) {                            // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
        pinMode(TFT_BL, OUTPUT);                 // Set backlight pin to output mode
        digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);  // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
    }

    tft.setSwapBytes(true);
    // tft.pushImage(0, 0,  240, 135, ttgo);
    // espDelay(5000);

    // tft.setRotation(0);
    // tft.fillScreen(TFT_RED);
    // espDelay(100);
    // tft.fillScreen(TFT_BLUE);
    // espDelay(100);
    // tft.fillScreen(TFT_GREEN);
    // espDelay(100);

    button_init();

    esp_adc_cal_characteristics_t adc_chars;
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize((adc_unit_t)ADC_UNIT_1, (adc_atten_t)ADC1_CHANNEL_6, (adc_bits_width_t)ADC_WIDTH_BIT_12, 1100, &adc_chars);
    //Check type of calibration value used to characterize ADC
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        Serial.printf("eFuse Vref:%u mV", adc_chars.vref);
        vref = adc_chars.vref;
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        Serial.printf("Two Point --> coeff_a:%umV coeff_b:%umV\n", adc_chars.coeff_a, adc_chars.coeff_b);
    } else {
        Serial.println("Default Vref: 1100mV");
    }

    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("LeftBtn:", tft.width() / 2, tft.height() / 2 - 32);
    tft.drawString("[WiFi Scan]", tft.width() / 2, tft.height() / 2 - 16);
    tft.drawString("RightBtn:", tft.width() / 2, tft.height() / 2);
    tft.drawString("[Voltage Monitor]", tft.width() / 2, tft.height() / 2 + 16);
    tft.drawString("RightBtnLongPress:", tft.width() / 2, tft.height() / 2 + 32);
    tft.drawString("[Deep Sleep]", tft.width() / 2, tft.height() / 2 + 48);
    tft.setTextDatum(TL_DATUM);
}

void sensorInit(){
  Serial.println("-->[PMSensor] Starting Panasonic sensor..");
  delay(100);
  hpmaSerial.begin(9600,SERIAL_8N1,HPMA_RX,HPMA_TX);
  delay(100);
}

void wrongDataState(){
  Serial.print("-->[E][HPMA] !wrong data!");
  hpmaSerial.end();
  sensorInit();
  delay(500);
}

char getLoaderChar(){ 
  char loader[] = {'/','|','\\','-'};
  return loader[random(0,4)];
}

void showValues(uint16_t pm25, uint16_t pm10) {
    char output[22];
    sprintf(output, "PM25:%03d PM10:%03d", pm25, pm10);
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(2);
    tft.drawString(String(output), tft.width() / 2, tft.height() / 2 - 24);
}

void sensorLoop() {
    int try_sensor_read = 0;
    String txtMsg = "";
    while (txtMsg.length() < 32 && try_sensor_read++ < SENSOR_RETRY) {
        while (hpmaSerial.available() > 0) {
            char inChar = hpmaSerial.read();
            txtMsg += inChar;
            Serial.print("-->[HPMA] read " + String(getLoaderChar()) + "\r");
        }
        Serial.print("-->[HPMA] read " + String(getLoaderChar()) + "\r");
    }
    if (try_sensor_read > SENSOR_RETRY) {
        Serial.println("-->[HPMA] read > fail!");
        Serial.println("-->[E][HPMA] disconnected ?");
        delay(500);  // waiting for sensor..
    }

    if (txtMsg[0] == 02) {
        Serial.print("-->[HPMA] read > done!");
        unsigned int pm25 = txtMsg[6] * 256 + byte(txtMsg[5]);
        unsigned int pm10 = txtMsg[10] * 256 + byte(txtMsg[9]);
        if (pm25 < 1000 && pm10 < 1000) {
            showValues(pm25, pm10);
        } else
            wrongDataState();
    } else
        wrongDataState();
}

void loop() {
    if (count++ > 5000000) suspend();
    if (btn1click) showVoltage();
    button_loop();
}
