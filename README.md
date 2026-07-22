## tiny farm: automatically cultivation with monitoring with esp32 
![board](https://img.shields.io/badge/Esp32-black.svg)
![code](https://img.shields.io/badge/C++-pink.svg)
![time](https://img.shields.io/badge/Hacklub-red.svg)
![prog](https://img.shields.io/badge/Macondo-green.svg)
# What is that?? 
A room temperature checker whose initial function. tbh this project especially for maggot cultivation and automatically controlling the lights at night, then there is also a room temperature sensor that will be connected to a fan when the conditions inside are very humid
## ![photo](https://user-cdn.hackclub-assets.com/019f87c9-898f-7c91-a521-dd761bbdac52/1000571051.jpg) 
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

## firmware build & flashing instructions

1. Install [Arduino IDE](https://www.arduino.cc/en/software) or use VS Code with PlatformIO.
2. add ESP32 Board URL to Preferences: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
3. install required libraries via Library Manager:
- OneWire
- DallasTemperature
- LiquidCrystal_I2C
4. Select ESP32 Dev Module as the board and pick your COM port.
5. **upload!!!**

# DS18B20
| DS18B20 | ESP32 |
|---------|-------|
| VCC | 3.3V |
| GND | GND |
| DATA | GPIO 18 |

# smoke sensor
| Sensor | ESP32 |
|--------|-------|
| VCC | 5V |
| GND | GND |
| DOUT | GPIO 16 |

# buzzer
| Buzzer | ESP32 |
|--------|-------|
| + | GPIO 2 |
| - | GND |

# LCD I2c
| LCD | ESP32 |
|-----|-------|
| VCC | 5V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

# infrared sensor
| Sensor | ESP32 |
|--------|-------|
| VCC | 3.3V |
| GND | GND |
| DOUT | GPIO 4 |

## Thanks for
<p align="left">
  <img src="https://assets.hackclub.com/flag-standalone.svg" width="120" style="display:inline-block; margin-right:10px;" />
  <img src="https://assets.hackclub.com/icon-square.svg" width="120" style="display:inline-block; margin-right:10px;" />
</p> 