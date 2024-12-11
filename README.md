## Hardware:
- Esp32-s3-WROOM-1 board
- WS2812B led strip
- Mobile phone with android

## Building esp32 application (esp/)
### First time on new machine:
- Follow these instructions: https://github.com/hekciu/esp32-probe

### If you have environment set up
- cd esp
- source ./init.sh
- idf.py -p PORT flash monitor # replace PORT with real one from /dev/

## Building mobile app(mobile/)
- sudo ./init.sh
#### Fill src/com/example/leds_mobile/Secrets.java with esp32 mac address
- sudo ./build.sh
### connect phone with developer tools & USB debugging enabled
- ./run.sh
- ./monitor.sh
