package com.example.leds_mobile;

import android.app.Activity;
import android.os.Bundle;
import android.widget.Button;
import android.widget.LinearLayout;
import android.view.View;

public class MainActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button buttonRed = findViewById(R.id.buttonRed);
        Button buttonBlue = findViewById(R.id.buttonBlue);
        Button buttonGreen = findViewById(R.id.buttonGreen);
        Button buttonPurple = findViewById(R.id.buttonPurple);

        LinearLayout backgroundLayout = findViewById(R.id.backgroundLayout);
        backgroundLayout.setBackgroundColor(Color.GREY);
    }
}
