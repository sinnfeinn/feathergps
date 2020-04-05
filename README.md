# feathergps
A esp32 hosted leaflet.js realtime gps tracker using:
Sparkfun Things Plus, ublox SAM-M8Q, leaflet.js, ESPAsync Webserver, SPIFFS

![Tracker](https://user-images.githubusercontent.com/936824/78511087-d6049380-774e-11ea-8e8b-2b6f3e1b356a.png)

## Hardware:
- SparkFun Thing Plus - ESP32 WROOM (https://www.sparkfun.com/products/15663)
- SparkFun GPS Breakout - Chip Antenna, SAM-M8Q (Qwiic) (https://www.sparkfun.com/products/15210)
- Qwiic Cable - 50mm (https://www.sparkfun.com/products/14426)

## Dependencies:
- https://github.com/me-no-dev/ESPAsyncWebServer
- https://github.com/sparkfun/SparkFun_Ublox_Arduino_Library
- https://arduinojson.org/
- https://github.com/me-no-dev/arduino-esp32fs-plugin

## Setup:
1. Connect ublox SAM-M8Q to Thing Plus via QWIIC
2. Edit Wifi Credentials in host.ino
3. Upload host.ino to Sparkfun Things Plus (https://learn.sparkfun.com/tutorials/esp32-thing-plus-hookup-guide)
4. Upload SPIFFS data folder (https://github.com/me-no-dev/arduino-esp32fs-plugin/releases/)
5. Open http://esp32.local in browser
6. Wait for GPS signal

## Todos: 
- Add Location Pin to leaflet realtime (currently using linestring)
- Add logging (https://www.sparkfun.com/products/13712)
- ???
