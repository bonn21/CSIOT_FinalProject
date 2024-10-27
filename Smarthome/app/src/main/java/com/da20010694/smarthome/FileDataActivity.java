package com.da20010694.smarthome;

import android.os.Bundle;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;

public class FileDataActivity extends AppCompatActivity {

    private TextView textViewData;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_file_data);

        textViewData = findViewById(R.id.textViewData);

        // Nhận tên tệp từ Intent
        String fileName = getIntent().getStringExtra("fileName");
        if (fileName != null) {
            loadFileData(fileName); // Gọi hàm để tải dữ liệu từ tệp
        } else {
            Toast.makeText(this, "Tên tệp không hợp lệ!", Toast.LENGTH_SHORT).show();
        }
    }

    private void loadFileData(String fileName) {
        File file = new File(getExternalFilesDir(null), fileName); // Đường dẫn tệp
        StringBuilder stringBuilder = new StringBuilder();

        try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
            String line;
            while ((line = reader.readLine()) != null) {
                stringBuilder.append(line).append("\n"); // Đọc từng dòng và thêm vào StringBuilder
            }
            textViewData.setText(stringBuilder.toString()); // Hiển thị dữ liệu lên TextView
        } catch (IOException e) {
            Toast.makeText(this, "Lỗi khi đọc dữ liệu tệp: " + e.getMessage(), Toast.LENGTH_SHORT).show();
        }
    }
}
