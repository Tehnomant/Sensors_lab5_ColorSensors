# Lab 5: Color Sensor with ESP32

This repository contains the code for Lab 5: Color Sensor, implemented on ESP32 using PlatformIO (Arduino framework).

## Main Task (`Lab-5/src/main.cpp`)
- Measures reflected light by sequentially turning on red, green, and blue LEDs
- Compares readings with ambient light to determine the dominant color
- Detects RED, GREEN, BLUE colors using gain calibration

## Additional Task (`Lab-5_Additional_task/src/main.cpp`)
- Implements RGB to HSV color space conversion
- Auto-calibrates using white reference for accurate readings
- Detects 6+ colors: RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE, plus BLACK/WHITE/GRAY based on saturation and value