# ESP32 temperature display

![ESP32 temperature display in action](temp-display.jpg?raw=true "ESP32 temperature display in action")

This is a simple Arduino project containing a few hardware parts:

* ESP32
* Serial display 7-segment 4 digits TM1637

On the software side we use [open weather map api](https://openweathermap.org/api).


Setup:

```bash
cp settings_default.h settings.h
```

Fill in your settings. Use the Arduino IDE do compile and flash the code.

## Problems?

Try following [this tutorial](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/).

Most of the time for me, the error has to do with not using a data USB cable.

Settings for ESP32 devkit 1:

* Board: DOIT ESP32 DEVKIT V1
* Upload Speed: 921600
* Flash Frequency: 80MHz
* Core Debug Level: None
* Board: /dev/ttyUSB0 (have also used ttyUSB5)
