#  Thermostat

This is for a thermostat that currently runs on an Arduino Uno R3. It
utilizes an XBee Series 2 radio, an RGB LCD display from Adafruit, and a
pre-built four relay board for activating different parts of the system.

A remote temperature sensor sends the temperature to the thermostat over the
XBee radio. This provides the ability relocate where the temperature is
measured that determines what the system will do.

Currently the remote temperature sensor is a Raspberry Pi Zero W using a
DS18B20 temperature sensor.

# Hardware

## Large Components
- Arduino Uno R3
- [RGB LCD Shield Kit w/ 16x2 Character Display, Positive Display](https://www.adafruit.com/product/716)
- [4-Channel 5V Relay Module](https://www.sainsmart.com/products/4-channel-5v-relay-module)
- [XBee Series 2 Radio](https://www.mouser.com/ProductDetail/DIGI/XB24CZ7PIT-004?qs=sGAEpiMZZMtJacPDJcUJYw9DFeLukfx0XzSgRHuYnDENHDxd6SSvAw%3D%3D)
- [XBee Shield V2.0](https://www.seeedstudio.com/XBee-Shield-V2-0-p-1375.html)

## Small Components
- Two bread boards
- Jumper wires
- Voltage regulator, 5V, to power the relays
- Capacitors
