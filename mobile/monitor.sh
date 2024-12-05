export SDK="/usr/lib/proprietary/google-android-sdk"
export BUILD_TOOLS="$SDK/build-tools/34.0.0"
export PLATFORM="$SDK/platforms/android-34"
"$SDK/platform-tools/adb" shell run-as com.example.leds_mobile logcat hekciu_leds:V *:S
