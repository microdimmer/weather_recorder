# Arduino weather recorder

Clock with

## Components

* Arduino Nano
* Aosong AM2320
* Nokia 5110 LCD display
* RTC DS3231
* SD card reader
* wires, enclosure

## Photos:
![PHOTO1](https://github.com/microdimmer/weather_recorder/blob/master/readme/front.jpg)
![PHOTO2](https://github.com/microdimmer/weather_recorder/blob/master/readme/disassembled.jpg)

## Libraries

* [Arduino-esp8266](https://github.com/esp8266/Arduino)
* [Blynk](https://github.com/blynkkk/blynk-library)
* [u8g2](https://github.com/olikraus/u8g2)
* [Adafruit_BME280_Library](https://github.com/adafruit/Adafruit_BME280_Library)
* [WiFiManager](https://github.com/tzapu/WiFiManager)
* [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
* [AsyncPing](https://github.com/akaJes/AsyncPing)
* [SimpleTimer](http://playground.arduino.cc/Code/SimpleTimer)
* [TimeLibrary](https://github.com/PaulStoffregen/Time)

## Wiring:
```
ST7920
RS - D8
R/W - D7
E - D6
backlight - D3

mh-z19
RX - D4
TX - D5

BME-280
SDA - D1
SCL - D2
VCC - 3V3
```

## Scheme:
![СХЕМА](https://github.com/microdimmer/homeweather_esp8266/blob/master/readme/scheme.png)

## Known issues:

* Temperature measurements seems to be higher due to heat from ESP8266
* ST7920 plate need to be cutted to fit enclosure
* Can't connect to my old ASUS WL-500gP V2

# Russian:
Часы с синхронизацией времени по WI-FI, функцией измерения CO2, температуры, влажности, давления. Также есть автоматическое изменение подсветки в зависимости от освещения.
Часы синхронизируются по NTP протоколу через WI-FI. Данные датчиков передаются на сервер Blynk.

## Компоненты

* Датчик CO2 MH-Z19
* NodeMCU v2
* ST7920 128x64 LCD-дисплей
* датчик давления/влажности/температуры BME280
* фоторезистор 5528 LDR
* кнопки, резисторы, провода, корпус

## Библиотеки

* [Arduino-esp8266](https://github.com/esp8266/Arduino)
* [Blynk](https://github.com/blynkkk/blynk-library)
* [u8g2](https://github.com/olikraus/u8g2)
* [Adafruit_BME280_Library](https://github.com/adafruit/Adafruit_BME280_Library)
* [WiFiManager](https://github.com/tzapu/WiFiManager)
* [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
* [AsyncPing](https://github.com/akaJes/AsyncPing)
* [SimpleTimer](http://playground.arduino.cc/Code/SimpleTimer)
* [TimeLibrary](https://github.com/PaulStoffregen/Time)

## Подключение:
```
дисплей 12864 
RS - D8
R/W - D7
E - D6
катод подсветки - D3

датчик mh-z19
RX - D4
TX - D5

датчик BME-280
SDA - D1
SCL - D2
VCC - 3V3
```
## Известные проблемы:

* ESP8266 греется и вносит погрешность в показания температуры, пока изолировал датчик BME-280 вспененным полиэтиленом
* Пришлось немного подрезать плату дисплея ST7920 сверху, не входила по высоте
* Не удалось подключить к роутеру ASUS WL-500gP V2 (видимо нужно разбираться с типом WI-FI сети на роутере)