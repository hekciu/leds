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
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothGattCharacteristic;
import android.content.Intent;
import android.os.IBinder;
import android.os.Handler;
import android.util.Log;


public class BluetoothService extends Service {
    private BluetoothManager manager;
    private BluetoothAdapter adapter;
    private BluetoothLeScanner scanner;
    private String espMacAddress;
    private String rgbCharacteristicUuid = "0xFF01";
    private BluetoothGatt bluetoothGatt = null;
    private BluetoothGattCharacteristic rgbCharacteristic = null;
    private String rgbString = "0x00,0x00,0xFF";
    private static final String LOG_TAG = "hekciu_leds";
    private static final int WRITE_INTERVAL_MS = 100;

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
                serviceInstance.handleScanResult(result);
            }
        
            @Override
            public void onBatchScanResults(List<ScanResult> results) {
                for (ScanResult result : results) {
                    serviceInstance.handleScanResult(result);
                }
            }
        }; 

        return scanCallback;
    }

    private void setRgbUpdateInterval() {
        Handler handler = new Handler();
    
        BluetoothService serviceInstance = this;

        Runnable updateData = new Runnable(){
            @Override
            public void run() {
                if (serviceInstance.rgbCharacteristic == null || serviceInstance.bluetoothGatt == null) {
                    return; 
                }

                try {
                    Log.d(LOG_TAG, "writing to esp, rgb string: " + serviceInstance.rgbString);

                    serviceInstance.bluetoothGatt.writeCharacteristic(serviceInstance.rgbCharacteristic, serviceInstance.rgbString.getBytes(), BluetoothGattCharacteristic.WRITE_TYPE_NO_RESPONSE);
                } catch (Exception e) {
                    Log.d(LOG_TAG, "an error occured while writing rgb string bytes to characteristic");
                    Log.d(LOG_TAG, "error details: "+e.toString());
                }

                handler.postDelayed(this, WRITE_INTERVAL_MS);
            }
        };

        handler.post(updateData);
    }

    private void handleScanResult(ScanResult result) {
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

            @Override 
            public void onServicesDiscovered(BluetoothGatt gatt, int status) {
                Log.d(LOG_TAG, "services discovered");

                if (status == BluetoothGatt.GATT_SUCCESS) {
                    List<BluetoothGattService> services = gatt.getServices();
                    for (BluetoothGattService service : services) {
                        List<BluetoothGattCharacteristic> characteristics = service.getCharacteristics();
                        for (BluetoothGattCharacteristic characteristic : characteristics) { 
                            Log.d(LOG_TAG, "got: " + characteristic.getUuid() + " wanted: " + serviceInstance.rgbCharacteristicUuid);

                            if (characteristic.getUuid().equals(serviceInstance.rgbCharacteristicUuid)) {
                                Log.d(LOG_TAG, "found characteristic matching rgbCharacteristic uuid");
                                serviceInstance.rgbCharacteristic = characteristic;
                            }
                        }
                    }
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

        this.setRgbUpdateInterval();

        return START_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }
}


