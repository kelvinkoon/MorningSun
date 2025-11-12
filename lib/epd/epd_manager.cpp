#include "epd_manager.h"
#include "time.h"
#include <GxEPD2_BW.h>
#include <GxEPD2.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>

#include "../utils/small_icons.h"
#include "../utils/large_icons.h"

#define CLEAR_DAY_STR "clear-day"
#define CLEAR_NIGHT_STR "clear-night"
#define RAIN_STR "rain"
#define SNOW_STR "snow"
#define SLEET_STR "sleet"
#define WIND_STR "wind"
#define FOG_STR "fog"
#define CLOUDY_STR "cloudy"
#define PARTLY_CLOUDY_DAY_STR "partly-cloudy-day"
#define PARTLY_CLOUDY_NIGHT_STR "partly-cloudy-night"

struct WeatherBitmap {
    const char* name;
    const unsigned char* small;
    const unsigned char* large;
};

// Lookup table
constexpr WeatherBitmap weatherBitmaps[] = {
    {CLEAR_DAY_STR,               epd_bitmap_small_clear_day,               epd_bitmap_large_clear_day},
    {CLEAR_NIGHT_STR,             epd_bitmap_small_clear_night,             epd_bitmap_large_clear_night},
    {RAIN_STR,                     epd_bitmap_small_rain,                    epd_bitmap_large_rain},
    {SNOW_STR,                     epd_bitmap_small_snow,                    epd_bitmap_large_snow},
    {SLEET_STR,                    epd_bitmap_small_sleet,                   epd_bitmap_large_sleet},
    {WIND_STR,                     epd_bitmap_small_wind,                    epd_bitmap_large_wind},
    {FOG_STR,                      epd_bitmap_small_fog,                     epd_bitmap_large_fog},
    {CLOUDY_STR,                   epd_bitmap_small_cloudy,                  epd_bitmap_large_cloudy},
    {PARTLY_CLOUDY_DAY_STR,        epd_bitmap_small_partly_cloudy_day,      epd_bitmap_large_partly_cloudy_day},
    {PARTLY_CLOUDY_NIGHT_STR,      epd_bitmap_small_partly_cloudy_night,    epd_bitmap_large_partly_cloudy_night},
};

const unsigned char* getWeatherBitmap(const char* icon, bool large = false) {
    for (const auto& entry : weatherBitmaps) {
        if (strcmp(icon, entry.name) == 0) {
            return large ? entry.large : entry.small;
        }
    }
    return large ? epd_bitmap_large_refresh : epd_bitmap_small_refresh;
}

const unsigned char* getSmallWeatherBitmap(const char* icon) {
    return getWeatherBitmap(icon, false);
}

const unsigned char* getLargeWeatherBitmap(const char* icon) {
    return getWeatherBitmap(icon, true);
}

EpdManager::EpdManager(int cs_pin, int dc_pin, int rst_pin, int busy_pin):
    _cs_pin(cs_pin),
    _dc_pin(dc_pin),
    _rst_pin(rst_pin),
    _busy_pin(busy_pin),
    _display(GxEPD2_290_BS(SS, dc_pin, rst_pin, busy_pin)) {}

void EpdManager::init() {
    _display.init();
    _display.setRotation(1);
    _display.setFullWindow();
}

void EpdManager::clearScreen() {
    _display.firstPage();
    do {
        _display.fillScreen(GxEPD_WHITE);
    } while (_display.nextPage());
}

void EpdManager::drawWeather(const WeatherData& weather_data) {
    _display.firstPage();
    do {
        _display.fillScreen(GxEPD_WHITE);

        int screen_width  = _display.width();
        int screen_height = _display.height();

        CurrentWeatherData current_weather = weather_data.current;
        int left_width  = (screen_width * 2) / 3;
        int left_start_x = 0;

        int large_icon_size = 100;
        int small_icon_size = 35;

        int gap_between_pairs = 20;
        int gap_below_large   = 10;

        _display.setFont(&FreeMonoBold9pt7b);
        _display.setTextColor(GxEPD_BLACK);

        int large_icon_y = (screen_height / 2) - (large_icon_size / 2) - small_icon_size + 15;
        int pairs_y     = large_icon_y + large_icon_size + gap_below_large;

        int center_x = left_start_x + left_width / 2;

        int pair_width = small_icon_size + 40;
        int total_pair_width = pair_width * 2 + gap_between_pairs;
        int start_x = center_x - (total_pair_width / 2);

        int temp_icon_x   = start_x;
        int temp_text_x   = temp_icon_x + small_icon_size + 2;
        int precip_icon_x = temp_text_x + 38 + gap_between_pairs;
        int precip_text_x = precip_icon_x + small_icon_size + 2;

        _display.drawBitmap(center_x - large_icon_size / 2, large_icon_y,
                            getLargeWeatherBitmap(current_weather.icon),
                            large_icon_size, large_icon_size, GxEPD_BLACK);

        _display.drawBitmap(temp_icon_x, pairs_y - 15, epd_bitmap_small_thermometer,
                            small_icon_size, small_icon_size, GxEPD_BLACK);
        _display.setCursor(temp_text_x - 5, pairs_y + 5);
        _display.print(current_weather.temperature, 1); 
        _display.print("C");

        _display.drawBitmap(precip_icon_x - 5, pairs_y - 15, epd_bitmap_small_umbrella,
                            small_icon_size, small_icon_size, GxEPD_BLACK);
        _display.setCursor(precip_text_x - 5, pairs_y + 5);
        _display.print((int)(current_weather.precip_probability * 100));
        _display.print("%");

        _display.drawLine(left_width, 0, left_width, screen_height, GxEPD_BLACK);

        int right_start_x      = left_width + 10;
        int right_width       = screen_width - left_width - 10;
        int row_height        = 40;
        int top_offset        = 5;
        int forecast_icon_size = 35;

        for (int i = 0; i < NUM_HOURLY_READINGS; i++) {
            int y_base = top_offset + i * row_height;

            _display.drawBitmap(right_start_x, y_base,
                                getSmallWeatherBitmap(weather_data.hourly[i].icon),
                                forecast_icon_size, forecast_icon_size, GxEPD_BLACK);

            _display.setCursor(right_start_x + forecast_icon_size + 10, y_base + 12);
            _display.print("+");
            _display.print((i + 1) * 3);
            _display.print("h");

            _display.setCursor(right_start_x + forecast_icon_size + 10, y_base + 28);
            _display.print((int)round(weather_data.hourly[i].temperature));
            _display.print("C");

            if (i < NUM_HOURLY_READINGS - 1) {
                int line_y = y_base + row_height - 5;
                _display.drawLine(left_width + 5, line_y, screen_width, line_y, GxEPD_BLACK);
            }
        }

    } while (_display.nextPage());
    _display.hibernate();
}

