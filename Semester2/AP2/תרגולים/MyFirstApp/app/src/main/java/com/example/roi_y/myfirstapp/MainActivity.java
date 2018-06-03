package com.example.roi_y.myfirstapp;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {

    private EditText txt;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        txt = (EditText)findViewById(R.id.editText);
        View view = findViewById(R.id.mainLayout);
        // OR: View view = getWindow().getDecorView().getRootView();
        view.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                String location = "X: " + motionEvent.getX() + ", Y: " + motionEvent.getY();
                Toast.makeText(MainActivity.this, location, Toast.LENGTH_SHORT).show();
                return false;
            }
        });
    }

    public void showMessage(View view) {
        String msg = txt.getText().toString();

        Toast.makeText(this, msg, Toast.LENGTH_SHORT).show();
    }

    public void openActivity(View view) {
        Intent intent = new Intent(this, CanvasActivity.class);
        startActivity(intent);
    }
}
