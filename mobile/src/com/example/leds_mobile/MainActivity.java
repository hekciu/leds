package com.example.leds_mobile;

import android.app.Activity;
import android.os.Bundle;
import android.widget.Button;
import android.widget.LinearLayout;
import android.view.View;
import android.graphics.Color;
import android.content.Intent;
import android.util.Log;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothAdapter;
import android.content.ServiceConnection;
import android.content.ComponentName;
import android.content.Context;
import android.os.IBinder;


public class MainActivity extends Activity {
    private static final String LOG_TAG = "hekciu_leds";
    private static final int REQUEST_ENABLE_BT = 0;
    private BluetoothManager manager;
    private BluetoothAdapter adapter;
    private Intent bindBluetoothServiceIntent;

    BluetoothService bluetoothService = null;
    
    private ServiceConnection mConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName className, IBinder binder) {
            bluetoothService = ((BluetoothService.LocalBinder)binder).getService();
        }

        public void onServiceDisconnected(ComponentName className) {
            bluetoothService = null;
        }
    };

    public void changeColor(int r, int g, int b) {
        if (this.bluetoothService == null) {
            Log.d(LOG_TAG, "tried to send message to service, but this.bluetoothService is null");
            return;
        }

        this.bluetoothService.setRgbData(r, g, b);
        this.bluetoothService.sendData();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        this.manager = getSystemService(BluetoothManager.class);
        this.adapter = manager.getAdapter();
        this.bindBluetoothServiceIntent = new Intent(MainActivity.this, BluetoothService.class);

        setContentView(R.layout.activity_main);
        LinearLayout backgroundLayout = findViewById(R.id.backgroundLayout);

        int red = 0x50FF0000;
        int green = 0x5000FF00;
        int blue = 0x500000FF;
        int purple = 0x50FF00FF;
        int backgroundColor = 0x50808080;

        Button buttonRed = findViewById(R.id.buttonRed);
        buttonRed.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Log.d(LOG_TAG, "RED"); 
                changeColor(255, 0, 0);
                backgroundLayout.setBackgroundColor(red);
            }
        });
        Button buttonBlue = findViewById(R.id.buttonBlue);
        buttonBlue.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Log.d(LOG_TAG, "BLUE"); 
                changeColor(0, 0, 255);
                backgroundLayout.setBackgroundColor(blue);
            }
        });
        Button buttonGreen = findViewById(R.id.buttonGreen);
        buttonGreen.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Log.d(LOG_TAG, "GREEN"); 
                changeColor(0, 255, 0);
                backgroundLayout.setBackgroundColor(green);
            }
        });
        Button buttonPurple = findViewById(R.id.buttonPurple);
        buttonPurple.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Log.d(LOG_TAG, "PURPLE"); 
                changeColor(255, 0, 255);
                backgroundLayout.setBackgroundColor(purple);
            }
        });
        Button buttonReset = findViewById(R.id.buttonReset);
        buttonReset.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
               backgroundLayout.setBackgroundColor(backgroundColor);
            }
        });

        backgroundLayout.setBackgroundColor(backgroundColor);

        if (this.adapter == null) {
            Log.d(LOG_TAG, "Could not get bluetooth adapter");
            return;
        }

        Log.d(LOG_TAG, "Successfully got bluetooth adapter");

        if (!this.adapter.isEnabled()) {
            Intent enableBtIntent = new Intent(this.adapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        } else {
            bindService(this.bindBluetoothServiceIntent, mConnection, Context.BIND_AUTO_CREATE);
        }
    }

    @Override 
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        // idk why it's -1 on user's acceptance but it is what it is
        if (requestCode == REQUEST_ENABLE_BT && resultCode == -1) {
            Log.d(LOG_TAG, "Successfully enabled bluetooth");
            bindService(this.bindBluetoothServiceIntent, mConnection, Context.BIND_AUTO_CREATE);
        }
    }
}
