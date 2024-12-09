export SDK="/usr/lib/proprietary/google-android-sdk"
export BUILD_TOOLS="$SDK/build-tools/34.0.0"
export PLATFORM="$SDK/platforms/android-34"
mkdir -p $SDK
curl -LO https://dl.google.com/android/repository/build-tools_r34-linux.zip
mkdir "$SDK/build-tools"
unzip -q build-tools_r34-linux.zip -d "$SDK/build-tools"
mv "$SDK/build-tools/android-14" "$SDK/build-tools/34.0.0"
curl -LO https://dl.google.com/android/repository/platform-34-ext7_r02.zip
mkdir "$SDK/platforms"
unzip -q platform-34-ext7_r02.zip -d "$SDK/platforms"
curl -LO https://dl.google.com/android/repository/platform-tools_r34.0.5-linux.zip
unzip -q platform-tools_r34.0.5-linux.zip -d "$SDK"

echo "creating src/com/example/leds_mobile/Secrets.java file, you need to fill it with mac address of ESP"
cp src/com/example/leds_mobile/Secrets.java.template src/com/example/leds_mobile/Secrets.java
