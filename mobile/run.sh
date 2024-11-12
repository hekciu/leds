export SDK="/usr/lib/proprietary/google-android-sdk"
export BUILD_TOOLS="$SDK/build-tools/34.0.0"
export PLATFORM="$SDK/platforms/android-34"
"$SDK/platform-tools/adb" install -r build/LedsMobile.apk
"$SDK/platform-tools/adb" shell am start -n com.example.leds_mobile/.MainActivity
