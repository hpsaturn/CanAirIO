#include <gui.hpp>

#ifdef ENABLE_TFT
TFT_eSPI tft = TFT_eSPI(135, 240);  // Invoke custom library
#endif

bool icWifiOn, icBTPair, icDataOn, icFanOn;
uint32_t uptime;

void showMainPage(){
#ifdef ENABLE_TFT
    showBatteryStatus();
    showPMSValues();
    showBME680Values();
    showStatus();
#endif
}

void showBatteryStatus() {
#ifdef ENABLE_TFT
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
    float volts = battGetVoltage();
    String battery = "";
    if (battIsCharging()) {
        tft.setTextColor(TFT_GREEN);
        battery = "CHAR:" + String(battCalcPercentage(volts)) + "%";
    }
    else {
        int level = battCalcPercentage(volts);
        if (level < 30) tft.setTextColor(TFT_RED);
        battery = "BATT:" + String(level) + "%";
    } 
    tft.drawString(battery, 0, 0);
    tft.setTextColor(TFT_WHITE);
    tft.setTextDatum(TC_DATUM);
    char suptime[11];
    sprintf(suptime,"%010d",uptime);
    tft.drawString(suptime, tft.width()/2 + 5, 0);
    String voltage = "" + String(volts) + "v";
    tft.setTextDatum(TR_DATUM);
    tft.drawString(voltage, tft.width(), 0);
#endif
}

void showStatus() {
    char output[30];
    icFanOn = pmsensorIsEnable();
    sprintf(output, "%s %s %s %s", icBTPair ? "BT" : "", icWifiOn ? "WiFi" : "", icFanOn ? "Fan" : "", icDataOn ? "Up" : "");
#ifdef ENABLE_TFT
    tft.setTextDatum(BC_DATUM);
    tft.setTextSize(2);
    tft.drawLine(0, tft.height() - 19, tft.width(), tft.height() - 19, TFT_YELLOW);
    tft.drawRect(0,tft.height()-18,tft.width(),tft.height(),TFT_BLACK);  // clear status line
    tft.drawString(output, tft.width() / 2, tft.height());
#endif
}

void showUptime(uint32_t u) {
    uptime = u;
}

void showBTIcon(bool isPaired) {
    icBTPair = isPaired;
}

void showWifiIcon(bool isWifiOn) {
    icWifiOn = isWifiOn;
}

void showDataIcon(bool isDataOn) {
    icDataOn = isDataOn;
}


void showBME680Values() {
#ifdef ENABLE_TFT
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(1);
    tft.drawString("G:" + getFormatGas(), 0, tft.height() / 2 + 7);
    tft.drawString("A:" + getFormatAlt(), tft.width() / 2, tft.height() / 2 + 7);
    tft.drawString("P:" + getFormatPress(), tft.width(), tft.height() / 2 + 7);
    tft.setTextSize(2);
    tft.drawString("T:" + getFormatTemp(), 0, tft.height() / 2 + 24);
    tft.drawString("H:" + getFormatHum(), tft.width(), tft.height() / 2 + 24);
#endif
}

void showPMSValues() {
#ifdef ENABLE_TFT
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(7);
    if (getPM25() >= 20) tft.setTextColor(TFT_YELLOW);
    if (getPM25() >= 60) tft.setTextColor(TFT_ORANGE);
    if (getPM25() >= 100) tft.setTextColor(TFT_RED);
    tft.drawString(getStringPM25(), tft.width() / 2 + 15, tft.height() / 2 - 28);
    tft.setTextSize(1);
    tft.drawString("PM2.5", tft.width(), tft.height() / 2 - 13);
    tft.drawString("PM1/PM10:", 0, tft.height() / 2 - 39);
    tft.drawString(getStringPM1(), 0, tft.height() / 2 - 26);
    tft.drawString(getStringPM10(), 0, tft.height() / 2 - 13);
    tft.setTextColor(TFT_WHITE);
#endif
}

void showVoltagePage() {
    float volts = battGetVoltage();
    String voltage = "" + String(volts) + "v";
    Serial.printf("-->[UI] voltage: %s\n", voltage.c_str());
#ifdef ENABLE_TFT
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(TC_DATUM);
    tft.setTextSize(2);
    tft.drawString("Voltage:", tft.width() / 2, 0);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(6);
    tft.drawString(voltage, tft.width() / 2, tft.height() / 2);
    String percentage = "" + String(battCalcPercentage(volts)) + "%";
    tft.setTextSize(3);
    tft.drawString(percentage, tft.width() / 2, tft.height() / 2 + 44);
#endif
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

void displayOff() {
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
    tft.setRotation(1);
    tft.setSwapBytes(true);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(0, 0);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(2);
    if (TFT_BL > 0) {                            // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
        pinMode(TFT_BL, OUTPUT);                 // Set backlight pin to output mode
        digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);  // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
    }
#endif
}

void guiLoop() {
    static uint_fast64_t guiTimeStamp = 0; 
    if (millis() - guiTimeStamp > 5000) {
        guiTimeStamp = millis();
        showMainPage();
    }
}

