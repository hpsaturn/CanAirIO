#include <sleep.hpp>

//! Long time delay, it is recommended to use shallow sleep, which can effectively reduce the current consumption
void espShallowSleep(int ms) {
    // commented it for possible fix for issue: https://github.com/Xinyuan-LilyGO/TTGO-T-Display/issues/36
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_sleep_enable_timer_wakeup(ms * 1000);
    delay(200);
    esp_light_sleep_start();
}

void espButtonSleep(int ms){
    esp_sleep_enable_timer_wakeup(ms * 1000);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_35, 0);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_light_sleep_start();
}

void espDeepSleep() {
    Serial.println("-->[ESP32] DeepSleep..");
    //After using light sleep, you need to disable timer wake, because here use external IO port to wake up
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
    // esp_sleep_enable_ext1_wakeup(GPIO_SEL_35, ESP_EXT1_WAKEUP_ALL_LOW);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_35, 0);
    delay(200);
    esp_deep_sleep_start();
}