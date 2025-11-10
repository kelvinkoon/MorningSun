## Morning Sun

A minimalist e-ink weather display built for the ESP32, Arduino, and [PirateWeather](https://pirate-weather.apiable.io/). Refreshes every 15 minutes.

## Set-up

### Config

Wi-fi credentials and API key are configured by `setup_config.sh` and stored in `src/config.h`.

## Hardware

-   [ESP32 WROOM-32D](https://www.espressif.com/en/products/modules/esp32).
-   [Waveshare 2.9 Inch e-Paper Display module](https://www.waveshare.com/2.9inch-e-paper-module.htm)
-   [Adafruit Powerboost 1000C](https://learn.adafruit.com/adafruit-powerboost-1000c-load-share-usb-charge-boost/downloads)
-   [Lithium Ion Polymer Battery - 3.7V](https://www.adafruit.com/product/1578)

### Pin Configuration:

| Waveshare e-Paper | DOIT ESP32 DEVKIT V1 |
| ----------------- | -------------------- |
| BUSY_PIN          | GPIO_NUM_04          |
| RST_PIN           | GPIO_NUM_16          |
| DC_PIN            | GPIO_NUM_17          |
| CS_PIN            | GPIO_NUM_05          |
| CLK_PIN           | GPIO_NUM_18          |
| DIN_PIN           | GPIO_NUM_23          |
| VCC               | 5V                   |
| GND               | GND                  |

## Credits

-   [Erika Flowers](https://github.com/erikflowers/weather-icons), converted to bitmaps with [image2cpp](https://javl.github.io/image2cpp/) for icons
-   [GxEPD2](https://github.com/ZinggJM/GxEPD2) by `@ZinggJM` for e-ink fonts
-   [esp32-e-paper-weatherdisplay](https://github.com/henri98/esp32-e-paper-weatherdisplay/tree/master) for inspiration
