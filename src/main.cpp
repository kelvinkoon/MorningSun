#include <SPI.h>
#include <GxEPD2_BW.h>
#include <GxEPD2.h>
#include <Arduino.h>
#include <WiFi.h>

#include "weather_client.h"
#include "epd_manager.h"
#include "config.h"
 
#define uS_TO_S_FACTOR 1000000ULL    /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  15*60         /* Time ESP32 will go to sleep (in seconds) */

// Pin definitions
#define CS_PIN 5
#define DC_PIN 17
#define RST_PIN 16
#define BUSY_PIN 4

RTC_DATA_ATTR int bootCount = 0;
 
WeatherClient weather_client(API_KEY, LATITUDE, LONGITUDE);
EpdManager epd_manager(CS_PIN, DC_PIN, RST_PIN, BUSY_PIN);
 
void initWifi() {
    Serial.printf("Connecting to %s", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void setup() {
    Serial.begin(115200);
    delay(3000);

    bootCount++;
    Serial.println("Boot number: " + String(bootCount));

    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");

    Serial.println("Initializing WiFi...");
    initWifi();

    epd_manager.init();
    epd_manager.clearScreen();

    WeatherData weather_data = weather_client.getWeather();
    epd_manager.drawWeather(weather_data);

    Serial.println("Going to sleep now");
    Serial.flush();
    esp_deep_sleep_start();
}
 
void loop() {
    // Should never be called
}
