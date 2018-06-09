# Arduino SD card weather recorder

Weather station based on DS3231 with data recording function (humidity and temperature).
Humidity and temperature is measuring every 10 minutes. Every 1 hour data is recording to SD card.

## Components

* Arduino Nano
* Aosong AM2320
* Nokia 5110 LCD display
* RTC DS3231
* SD card reader
* wires, enclosure

## Photos:
![PHOTO1](https://github.com/microdimmer/weather_recorder/blob/master/readme/front.jpg)

## Libraries

* [Adafruit_Sensor](https://github.com/adafruit/Adafruit_Sensor)
* [SdFat](https://github.com/greiman/SdFat)
* [u8glib](https://github.com/olikraus/U8glib_Arduino)
* [DS3232RTC](https://github.com/JChristensen/DS3232RTC)
* [SimpleTimer](http://playground.arduino.cc/Code/SimpleTimer)
* [TimeLibrary](https://github.com/PaulStoffregen/Time)

## Scheme:
![СХЕМА](https://github.com/microdimmer/weather_recorder/blob/master/readme/scheme.png)

# Russian:
Погодная станция, основанная на точном модуле DS3231 с функцией записи температуры и влажности на SD-карту.
Измеряются показания температуры и влажности с привязкой ко времени каждые 10 минут, данные записываются пачкой каждый час.

## Компоненты

* Arduino Nano
* Датчик температуры и влажности Aosong AM2320
* Дисплей Nokia 5110 
* Модуль точного времени DS3231
* SD-кардридер
* провода, корпус

## Библиотеки

* [Adafruit_Sensor](https://github.com/adafruit/Adafruit_Sensor)
* [SdFat](https://github.com/greiman/SdFat)
* [u8glib](https://github.com/olikraus/U8glib_Arduino
* [DS3232RTC](https://github.com/JChristensen/DS3232RTC)
* [SimpleTimer](http://playground.arduino.cc/Code/SimpleTimer)
* [TimeLibrary](https://github.com/PaulStoffregen/Time)