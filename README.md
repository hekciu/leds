This repository contains both code that should be run on an esp32 microcontroller and code for building android application.
All building scripts were written and tested on debian.

## Mobile app looks like that:

<img src="/img/application_interface.jpg" alt="" width="200"/>

## Hardware:
- Esp32-s3-WROOM-1 board
- WS2812B led strip
- Mobile phone with android

## Building esp32 application (esp/)
### First time on new machine:
- Follow these instructions: https://github.com/hekciu/esp32-probe

### If you have environment set up
- source ./init.sh
- idf.py -p PORT flash monitor # replace PORT with real one from /dev/

## Building mobile app (mobile/)
### First time on new machine:
- sudo ./init.sh

### If you have environment set up
#### Fill src/com/example/leds_mobile/Secrets.java with esp32 mac address
- sudo ./build.sh

### connect phone with developer tools & USB debugging enabled
- ./run.sh
- ./monitor.sh
