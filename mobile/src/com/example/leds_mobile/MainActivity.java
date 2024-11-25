package com.example.leds_mobile;

import android.app.Activity;
import android.os.Bundle;
import android.widget.Button;
import android.widget.LinearLayout;
import android.view.View;
import android.graphics.Color;
import android.content.Intent;


public class MainActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

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
               backgroundLayout.setBackgroundColor(red);
            }
        });
        Button buttonBlue = findViewById(R.id.buttonBlue);
        buttonBlue.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
               backgroundLayout.setBackgroundColor(blue);
            }
        });
        Button buttonGreen = findViewById(R.id.buttonGreen);
        buttonGreen.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
               backgroundLayout.setBackgroundColor(green);
            }
        });
        Button buttonPurple = findViewById(R.id.buttonPurple);
        buttonPurple.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
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

        startService(new Intent(MainActivity.this, BluetoothService.class));
    }
}
