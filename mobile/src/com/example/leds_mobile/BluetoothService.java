package com.example.leds_mobile;

import java.util.List;

import android.app.Service;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;


public class BluetoothService extends Service {
    private BluetoothManager manager;
    private BluetoothAdapter adapter;
    private BluetoothLeScanner scanner;
    private String espMacAddress;
    private BluetoothGatt bluetoothGatt;
    private static final String LOG_TAG = "hekciu_leds";

    private ScanCallback scanCallback;

    private ScanCallback getScanCallback() {
        BluetoothService serviceInstance = this; 

        ScanCallback scanCallback = new ScanCallback() {
            @Override
            public void onScanFailed(int errorCode) {
                Log.d(LOG_TAG, "BLE scan failed with code: " + errorCode);
            }

            @Override
            public void onScanResult(int callbackType, ScanResult result) {
                serviceInstance._handleScanResult(result);
            }
        
            @Override
            public void onBatchScanResults(List<ScanResult> results) {
                for (ScanResult result : results) {
                    serviceInstance._handleScanResult(result);
                }
            }
        }; 

        return scanCallback;
    }

    private void _handleScanResult(ScanResult result) {
        BluetoothDevice device = result.getDevice();

        Log.d(LOG_TAG, "Found device with address: " + device.getAddress());

        if (!device.getAddress().equals(this.espMacAddress)) {
            return;
        }

        this.scanner.stopScan(this.scanCallback);

        Log.d(LOG_TAG, "Esp found");

        BluetoothService serviceInstance = this;

        this.bluetoothGatt = device.connectGatt(this, false, new BluetoothGattCallback() {
            @Override
            public void onConnectionStateChange(final BluetoothGatt gatt, final int status, final int newState) {
                if (newState == BluetoothProfile.STATE_CONNECTED) {
                    gatt.discoverServices();
                } else {
                    gatt.close();

                    serviceInstance.scanner.startScan(serviceInstance.scanCallback);
                }
            }
        }, 2); // TRANSPORT_LE == 2

    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d(LOG_TAG, "Starting BluetoothService"); 
        this.manager = getSystemService(BluetoothManager.class);
        this.adapter = manager.getAdapter();
        this.scanner = this.adapter.getBluetoothLeScanner();
        this.scanCallback = this.getScanCallback();

        this.espMacAddress = Secrets.getAddress();

        this.scanner.startScan(this.scanCallback);

        return START_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }
}


