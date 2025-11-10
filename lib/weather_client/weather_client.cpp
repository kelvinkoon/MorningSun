#include "weather_client.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define URL_BUFFER_SIZE 256
#define JSON_BUFFER_SIZE 4096

#define DATA_STR "data"
#define TEMPERATURE_STR "temperature"
#define PRECIPITATION_PROB_STR "precipProbability"
#define ICON_STR "icon"
#define TIME_STR "time"

WeatherClient::WeatherClient(const char* api_key, float latitude, float longitude)
  : _api_key(api_key), _latitude(latitude), _longitude(longitude) {}

bool parseCurrentWeather(const JsonObject& currently, WeatherData& data) {
    if (currently.isNull()) {
        data.current.temperature = 0.0f;
        data.current.precip_probability = 0.0f;
        data.current.icon[0] = '\0';
        return false;
    };

    if (currently[TEMPERATURE_STR].is<float>()) {
        data.current.temperature = currently[TEMPERATURE_STR].as<float>();
    } else {
        data.current.temperature = 0.0f;
    }

    if (currently[PRECIPITATION_PROB_STR].is<float>()) {
        data.current.precip_probability = currently[PRECIPITATION_PROB_STR].as<float>();
    } else {
        data.current.precip_probability = 0.0f;
    }

    if (currently[ICON_STR].is<const char*>()) {
        const char* icon = currently[ICON_STR];
        strncpy(data.current.icon, icon, sizeof(data.current.icon));
        data.current.icon[sizeof(data.current.icon) - 1] = '\0'; // Ensure null termination
    } else {
        data.current.icon[0] = '\0'; // Default to empty string
    }

    return true;
}

bool parseHourlyWeather(const JsonObject& hourly, WeatherData& data) {
    if (hourly.isNull()) {
        for (int i = 0; i < NUM_HOURLY_READINGS; i++) {
            data.hourly[i].time = 0;
            data.hourly[i].temperature = 0.0f;
            data.hourly[i].icon[0] = '\0';
        }
        return false;
    }

    JsonArray hourly_data_arr = hourly[DATA_STR];
    if (hourly_data_arr.isNull()) {
        for (int i = 0; i < NUM_HOURLY_READINGS; i++) {
            data.hourly[i].time = 0;
            data.hourly[i].temperature = 0.0f;
            data.hourly[i].icon[0] = '\0';
        }
        return false;
    }

    for (int i = 0; i < NUM_HOURLY_READINGS; i++) {
        Serial.printf("Parsing hourly weather data for +%dh\n", (i + 1) * 3);

        const int three_hour_increment = 3;
        const int start_index = 3;
        int three_hour_data_index = start_index + i * three_hour_increment;

        if (three_hour_data_index >= hourly_data_arr.size()) {
            data.hourly[i].time = 0;
            data.hourly[i].temperature = 0.0f;
            data.hourly[i].icon[0] = '\0';
            continue;
        }

        JsonObject hour_data = hourly_data_arr[three_hour_data_index];
        if (hour_data.isNull()) {
            data.hourly[i].time = 0;
            data.hourly[i].temperature = 0.0f;
            data.hourly[i].icon[0] = '\0';
            continue;
        }

        // Safely assign each field with defaults
        data.hourly[i].time = hour_data[TIME_STR].is<long>()
            ? hour_data[TIME_STR].as<long>()
            : 0;

        data.hourly[i].temperature = hour_data[TEMPERATURE_STR].is<float>()
            ? hour_data[TEMPERATURE_STR].as<float>()
            : 0.0f;

        if (hour_data[ICON_STR].is<const char*>()) {
            const char* icon = hour_data[ICON_STR];
            strncpy(data.hourly[i].icon, icon, sizeof(data.hourly[i].icon));
            data.hourly[i].icon[sizeof(data.hourly[i].icon) - 1] = '\0';
        } else {
            data.hourly[i].icon[0] = '\0';
        }
    }

    return true;
}

// Returns true if the URL fits in the buffer, false otherwise
bool WeatherClient::_buildUrl(char* buffer, size_t buffer_size) const {
    int written = snprintf(
        buffer,
        buffer_size,
        "https://api.pirateweather.net/forecast/%s/%.6f,%.6f?units=si&exclude=minutely,daily",
        _api_key,
        _latitude,
        _longitude
    );

    return (written > 0 && written < buffer_size);
}

WeatherData WeatherClient::getWeather() {
    HTTPClient http;
    WeatherData data;

    char url[URL_BUFFER_SIZE];
    if (_buildUrl(url, sizeof(url)) == false) {
        Serial.println("URL buffer size is insufficient");
        http.end();
        return WeatherData(); // Return empty data on error
    }

    Serial.println("Sending HTTP request");
    http.begin(url);

    int http_code = http.GET();

    if (http_code == HTTP_CODE_OK) {
        WiFiClient& stream = http.getStream();
        DynamicJsonDocument doc(JSON_BUFFER_SIZE);
        DeserializationError error = deserializeJson(doc, stream);
        if (!error) {
            JsonObject currently = doc["currently"];
            if (!parseCurrentWeather(currently, data)) {
                Serial.println("Failed to parse 'currently' weather data, using defaults");
            }
            JsonObject hourly = doc["hourly"];
            if (!parseHourlyWeather(hourly, data)) {
                Serial.println("Failed to parse 'hourly' weather data, using defaults");
            }
        } else {
            Serial.println("Failed to parse JSON response: " + String(error.c_str()));
            // TODO: Retry on fail
        }
    } else {
        Serial.printf("HTTP client failed with HTTP code: %d\n", http_code);
    }

    http.end();
    return data;
}
