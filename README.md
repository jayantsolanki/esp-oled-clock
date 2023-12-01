# esp-oled-clock
ESP8266 12E Wemos D1 and 0.66" OLED based weather widget 

## Requirements:
* ESP12E Wemos
* 0.66" 64x40px OLED Shield for Wemos D1 [https://www.wemos.cc/en/latest/d1_mini_shield/oled_0_66.html]

## Reference

* [https://www.electronics-lab.com/project/esp8266-based-online-weather-widget/]
* [https://github.com/olikraus/u8g2/wiki/u8g2reference] for learning about drawing string. Focus on Fonts and drawStr method

## Dependencies
* API key from openweather
* Windows 10 or 7
* Arduino IDE
* Setup the IDE for flashing the ESP-12E. Follow this guide: [https://www.electronics-lab.com/project/getting-started-with-the-nodemcu-esp8266-based-development-board/]

## Instructions
Just fit the Oled Shield directly on the ESP, no need of any wiring or such

## Changes done
Original code in the reference is for the 1306 128x64px. So, made following changes:
* changed the constructor for OLED, added U8G2_SSD1306_64X48_ER_2_SW_I2C
* added HTTPClient library for ESP, original client object wasn't able to parse the json and was showing invalid query message
* changed some fonts and coordinates for drawing them, to better suit the smaller display
