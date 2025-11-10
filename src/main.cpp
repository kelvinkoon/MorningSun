#include <SPI.h>
#include <GxEPD2_BW.h>
#include <GxEPD2.h>
#include <Arduino.h>
#include <WiFi.h>

#include "weather_client.h"
#include "epd_manager.h"
#include "config.h"
 
// Pin definitions
#define CS_PIN 5
#define DC_PIN 17
#define RST_PIN 16
#define BUSY_PIN 4

const long interval_ms = 15*60*1000; // 15 minute
unsigned long prev_ms = 0;
 
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
    Serial.println("Initializing WiFi...");
    initWifi();

    epd_manager.init();
    epd_manager.clearScreen();

    WeatherData weather_data = weather_client.getWeather();
    epd_manager.drawWeather(weather_data);
}
 
void loop() {
    unsigned long current_ms = millis();
    if (current_ms - prev_ms >= interval_ms) {
        Serial.println("Current refresh time: " + String(current_ms));
        prev_ms = current_ms;
        epd_manager.clearScreen();
        WeatherData weather_data = weather_client.getWeather(); 
        epd_manager.drawWeather(weather_data);
    }
}
