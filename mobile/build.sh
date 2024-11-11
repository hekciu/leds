export PROJ=$(pwd)
sudo /opt/android-sdk/build-tools/26.0.1/aapt package -f -m -J $PROJ/src -M $PROJ/AndroidManifest.xml -S $PROJ/res -I /opt/android-sdk/platforms/android-19/android.jar
javac -d obj -classpath src -bootclasspath /opt/android-sdk/platforms/android-19/android.jar src/com/example/leds-mobile/*.java
sudo /opt/android-sdk/build-tools/26.0.1/dx --dex --output=$PROJ/bin/classes.dex $PROJ/obj
