export SDK="/usr/lib/proprietary/google-android-sdk"
export BUILD_TOOLS="$SDK/build-tools/34.0.0"
export PLATFORM="$SDK/platforms/android-34"
mkdir -p build/gen build/obj build/apk
sudo "$BUILD_TOOLS/aapt" package -f -m -J build/gen/ -S res -M AndroidManifest.xml -I "$PLATFORM/android.jar" && \
sudo javac --release 11 -classpath "$PLATFORM/android.jar" -d build/obj build/gen/com/example/leds_mobile/R.java src/com/example/leds_mobile/MainActivity.java src/com/example/leds_mobile/BluetoothService.java && \
sudo "$BUILD_TOOLS/d8" --release --lib "$PLATFORM/android.jar" --output build/apk/ build/obj/com/example/leds_mobile/*.class && \
sudo "$BUILD_TOOLS/aapt" package -f -M AndroidManifest.xml -S res/ -I "$PLATFORM/android.jar" -F build/LedsMobile.unsigned.apk build/apk/ && \
sudo "$BUILD_TOOLS/zipalign" -f -p 4 build/LedsMobile.unsigned.apk build/LedsMobile.aligned.apk && \
sudo "$BUILD_TOOLS/apksigner" sign --ks keystore.jks --ks-key-alias androidkey --ks-pass pass:android --key-pass pass:android --out build/LedsMobile.apk build/LedsMobile.aligned.apk
