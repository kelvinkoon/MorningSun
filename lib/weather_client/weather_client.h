#pragma once
#include <cstddef>

#define LEN_WEATHER_ICON 20
#define NUM_HOURLY_READINGS 3

struct CurrentWeatherData {
    char icon[LEN_WEATHER_ICON];
    float temperature;
    float precip_probability;
};

struct HourlyWeatherData {
    int time;
    char icon[LEN_WEATHER_ICON];
    float temperature;
};

struct WeatherData {
    CurrentWeatherData current;
    HourlyWeatherData hourly[NUM_HOURLY_READINGS];
};

class WeatherClient {
    public:
        WeatherClient(const char* _api_key, const float latitude, const float longitude);
    
        WeatherData getWeather();

    private:
        const char* _api_key;
        const float _latitude;
        const float _longitude;

        bool _buildUrl(char* buffer, size_t buffer_size) const;
};
