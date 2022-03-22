#include <bluetooth.hpp>

BLEServer* pServer = NULL;
BLECharacteristic* pCharactData = NULL;
BLECharacteristic* pCharactConfig = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

/*************************************************************************
*   B L U E T O O T H   P A Y L O A D
*************************************************************************/

String getNotificationData() {
    StaticJsonDocument<40> doc;
    doc["P25"] = getPM25();  // notification capacity is reduced, only main value
    String json;
    serializeJson(doc, json);
    return json;
}

String getSensorData() {
    StaticJsonDocument<150> doc;
    doc["P25"] = getPM25();
    doc["P10"] = getPM10();
    doc["lat"] = cfg.lat;
    doc["lon"] = cfg.lon;
    doc["alt"] = cfg.alt;
    doc["spd"] = cfg.spd;
    String json;
    serializeJson(doc, json);
    return json;
}

/*************************************************************************
*   B L U E T O O T H   M E T H O D S
*************************************************************************/

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        showBTIcon(true);
        Serial.println("-->[BLE] onConnect");
    };

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        showBTIcon(false);
        Serial.println("-->[BLE] onDisconnect");
    };
};  // BLEServerCallbacks

class MyConfigCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        if (value.length() > 0) {
            if (cfg.save(value.c_str())) {
                cfg.reload();
                if (cfg.isNewWifi) {
                    wifiRestart();
                    apiInit();
                    influxDbInit();
                }
                if (cfg.isNewIfxdbConfig) influxDbInit();
                if (cfg.isNewAPIConfig) apiInit();
                if (!cfg.wifiEnable) wifiStop();
            } 
            pCharactConfig->setValue(cfg.getCurrentConfig().c_str());
            pCharactData->setValue(getSensorData().c_str());
        }
    }
};

void bleServerInit() {
    // Create the BLE Device
    BLEDevice::init("CanAirIO_ESP32");
    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    // Create the BLE Service
    BLEService* pService = pServer->createService(SERVICE_UUID);
    // Create a BLE Characteristic for PM 2.5
    pCharactData = pService->createCharacteristic(
        CHARAC_DATA_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    // Create a BLE Characteristic for Sensor mode: STATIC/MOVIL
    pCharactConfig = pService->createCharacteristic(
        CHARAC_CONFIG_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    // Create a Data Descriptor (for notifications)
    pCharactData->addDescriptor(new BLE2902());
    // Saved current sensor data
    pCharactData->setValue(getSensorData().c_str());
    // Setting Config callback
    pCharactConfig->setCallbacks(new MyConfigCallbacks());
    // Saved current config data
    pCharactConfig->setValue(cfg.getCurrentConfig().c_str());
    // Start the service
    pService->start();
    // Start advertising
    pServer->getAdvertising()->start();
    Serial.println("-->[BLE] GATT server ready. (Waiting for client)");
}

void bleLoop() {
    static uint_fast64_t bleTimeStamp = 0;
    // notify changed value
    if (deviceConnected && pmsensorDataReady() && (millis() - bleTimeStamp > 5000)) {  // each 5 secs
        Serial.println("-->[BLE] sending notification..");
        bleTimeStamp = millis();
        pCharactData->setValue(getNotificationData().c_str());  // small payload for notification
        pCharactData->notify();
        pCharactData->setValue(getSensorData().c_str());  // load big payload for possible read
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(250);                   // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising();  // restart advertising
        Serial.println("-->[BLE] start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}

bool bleIsConnected(){
    return deviceConnected;
}
