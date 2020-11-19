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
