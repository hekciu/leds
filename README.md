## Hardware:
- Esp32-s3-WROOM-1 board
- WS2812B led strip
- Mobile phone with android

## Building esp32 application (debian)
### First time on new machine:
- Follow these instructions: https://github.com/hekciu/esp32-probe

### If you have environment set up
- cd esp
- source ./init.sh
- idf.py -p PORT flash monitor # replace PORT with real one from /dev/

## Building mobile app'
- sudo apt-get install openjdk-8-jdk-headless
- wget https://dl.google.com/android/repository/sdk-tools-linux-3859397.zip
- sudo mkdir -p /opt/android-sdk
- sudo unzip sdk-tools-linux-3859397.zip -d /opt/android-sdk
- rm sdk-tools-linux-3859397.zip
- sudo apt-get install adb
