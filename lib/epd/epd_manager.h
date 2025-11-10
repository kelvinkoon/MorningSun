#pragma once
#include <GxEPD2_BW.h>
#include <GxEPD2.h>

#include "weather_client.h" 

class EpdManager {
    public:
        EpdManager(int cs_pin, int dc_pin, int rst_pin, int busy_pin);

        void init();

        void clearScreen();

        void drawWeather(const WeatherData& weather_data);

    private:
        const int _cs_pin;
        const int _dc_pin;
        const int _rst_pin;
        const int _busy_pin;

        GxEPD2_BW<GxEPD2_290_BS, GxEPD2_290_BS::HEIGHT> _display;
};