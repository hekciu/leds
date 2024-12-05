package com.example.leds_mobile;

import android.app.Service;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;


public class BluetoothService extends Service {
    private BluetoothManager manager;
    private BluetoothAdapter adapter;
    private static final String LOG_TAG = "hekciu_leds";
    private static final int REQUEST_ENABLE_BT = 0;

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {

        return START_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }
}


