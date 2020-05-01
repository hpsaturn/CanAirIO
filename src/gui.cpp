#include <gui.hpp>

#ifdef ENABLE_TFT
TFT_eSPI tft = TFT_eSPI(135, 240);  // Invoke custom library
#endif

void showMainPage(){
    showBatteryStatus();
    showPMSValues();
    showBME680Values();
}

void showBatteryStatus() {
#ifdef ENABLE_TFT
    float volts = battGetVoltage();
    String voltage = "" + String(volts) + "v";
    String battery = "BATT:" + String(battCalcPercentage(volts)) + "%";
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
    tft.drawString(battery, 0, 0);
    tft.setTextDatum(TR_DATUM);
    tft.drawString(voltage, tft.width(), 0);
#endif
}

void showBME680Values() {
#ifdef ENABLE_TFT
    tft.setTextSize(2);
    tft.setTextDatum(BL_DATUM);
    String outln1 = "T:" + getTemperature() + " HR:" + getHumidity();
    tft.drawString(outln1, 0, tft.height() - 36);
    String outln2 = "P:" + getPressure() + " A:" + getAltitude();
    tft.drawString(outln2, 0, tft.height() - 18);
    String outln3 = "GAS:" + getGas();
    tft.drawString(outln3, 0, tft.height());
#endif
}

void showPMSValues() {
#ifdef ENABLE_TFT
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(8);
    tft.drawString(getStringPM25(), tft.width() / 2, tft.height() / 2 - 24);
    tft.setTextSize(1);
    tft.drawString("PM2.5", tft.width() / 2 + 75, tft.height() / 2 - 4);
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

void guiLoop() {
    static uint64_t timeStamp = 0;
    if (millis() - timeStamp > 3000) {
        timeStamp = millis();
        showMainPage();
    }
}