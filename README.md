# API for ESP8266 Shelly Relay TM1814 LED Controller

API for usage with esp8266 based Shelly relay and TM1814 LED strips. These are 12/24V (3/6 LEDs per pixel) RGB+White/Warm White. Chip is external so bend with caution.

Power usage when setting strip to all zero (black) is very high, so I am utilizing a Shelly relay device to fully cut power and the RX pin for singal. Only RX pin is capable of a reverse signal necessary for this LED strip type.
- You may encounter issues with first pixel (3/6 LEDs) in long lengths using a 3.3v signal.

## Setup
- Create an arduino_secrets.h file or copy the example and update your SSID and password.
- Using a USB to Serial device you'll need to flash your shelly relay with Arduino IDE and jumpers to the exposed pins
- You may need to ground GPIO0 on power up, then remove to show up as a device in IDE
- You may need to provide external 3.3v and ground to USB to Serial device if it isn't providing enough stable current
- Ensure jumper is correctly set to 12V and connect to 12V power supply per Shelly instructions
    - I have not tested 24V strands that require jumper on other side, it may affect being able to use GPIO RX 
- API endpoints can be hit with postman or custom software
    - See api.ino for list of get and put commands available
