## tiny farm: automatically cultivation with monitoring with esp32 
![board](https://img.shields.io/badge/Esp32-black.svg)
![code](https://img.shields.io/badge/C++-pink.svg)
![time](https://img.shields.io/badge/Hacklub-red.svg)
![prog](https://img.shields.io/badge/Macondo-green.svg)
# What is that?? 
A room temperature checker whose initial function. tbh this project especially for maggot cultivation and automatically controlling the lights at night, then there is also a room temperature sensor that will be connected to a fan when the conditions inside are very humid
## ![photo](https://i.imgur.com/vCoHjpQ.jpeg) 
# tools and materials
- 1x ESP32 dev module
- 1x buzzer
- 1x Sensor DS18B20
- 2x Relay (1 relay for fan)
- 1x light sensor
- 1x I2c display
- 1x smoke sensor
- jumper wires 
## ![photo](https://cache.paltena.com/base/1280x892/96c93cfb-bede-4a17-bdcc-3311918ebdbd.webp) 
> The photo above is not related to what i doing because many sensors are not available in either Wokwi or tinkercad bt this is for informational
| DS18B20 | ESP32 |
|---------|-------|
| VCC | 3.3V |
| GND | GND |
| DATA | GPIO 18 |
| DATA ↔ VCC |