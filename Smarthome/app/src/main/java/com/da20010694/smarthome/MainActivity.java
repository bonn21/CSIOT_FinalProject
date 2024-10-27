package com.da20010694.smarthome;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;
import java.io.OutputStreamWriter;
import android.os.Environment;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;
import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;

import java.io.File;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Locale;

public class MainActivity extends AppCompatActivity {

    private TextView tvTemperature, tvHumidity, tvGas;

    private FileOutputStream fileOutputStream; // Đối tượng FileOutputStream để ghi dữ liệu
    private String fileName; // Tên tệp
    private ProgressBar temperatureProgressBar, humidityProgressBar, gasProgressBar;
    private DatabaseReference mDatabase;
    private DatabaseReference commandReference; // Khối command
    private ImageView imageViewKhoacua; // Khai báo ImageView cho cửa
    private Button buttonCua; // Khai báo nút mở/đóng cửa

    private LineChart lineChart;
    private ArrayList<Entry> temperatureEntries = new ArrayList<>();
    private ArrayList<Entry> humidityEntries = new ArrayList<>();
    private ArrayList<Entry> gasEntries = new ArrayList<>();
    private int entryIndex = 0;
    private Button btnLuuTru;
    boolean isLoadState;
    private int fileCounter = 1; // Biến đếm số lượng tệp đã tạo




    private boolean isCuaOpen = false; // Biến lưu trạng thái cửa

    private DatabaseReference databaseReference;
    private ValueEventListener valueEventListener;
    private boolean isListening = false;
    private int fileCount = 1;

    private Button buttonReset;

    private boolean isStoring = false; // Biến kiểm tra trạng thái lưu trữ

    private FileOutputStream fos; // Biến để lưu trữ FileOutputStream


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Khởi tạo các thành phần giao diện
        tvTemperature = findViewById(R.id.nhietdo1);
        tvHumidity = findViewById(R.id.doam1);
        tvGas = findViewById(R.id.khiga1);
        temperatureProgressBar = findViewById(R.id.nhietdo);
        humidityProgressBar = findViewById(R.id.doam);
        gasProgressBar = findViewById(R.id.khiga);
        lineChart = findViewById(R.id.lineChart);
        imageViewKhoacua = findViewById(R.id.bieutuongkhoacua); // Khởi tạo ImageView cho cửa
        buttonCua = findViewById(R.id.buttoncua); // Khởi tạo nút mở/đóng cửa
        btnLuuTru = findViewById(R.id.luutru);

        Intent serviceIntent = new Intent(this, GasTempAlertService.class);
        startService(serviceIntent);

        // Khởi tạo các nút Phòng Chính, Phòng Ngủ, và Khu Vườn
        Button buttonPhongChinh = findViewById(R.id.buttonphongchinh);
        Button buttonPhongNgu = findViewById(R.id.buttonphongngu);


        // Khởi tạo các tham chiếu đến Firebase
        commandReference = FirebaseDatabase.getInstance().getReference("command");
        mDatabase = FirebaseDatabase.getInstance().getReference("DHTData");

        buttonReset = findViewById(R.id.reset);
        buttonReset.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Kiểm tra trạng thái và thực hiện hành động phù hợp
                if (isLoadState) {
                    // Nếu đang ở trạng thái "LOAD LINE", gọi hàm để cập nhật biểu đồ
                    updateLineChart(); // Gọi hàm để cập nhật biểu đồ
                    buttonReset.setText("OFF LINE"); // Cập nhật văn bản của nút
                } else {
                    // Nếu đang ở trạng thái "OFF LINE", gọi hàm để reset biểu đồ
                    resetLineChart(); // Gọi hàm để reset biểu đồ
                    buttonReset.setText("LOAD LINE"); // Cập nhật văn bản của nút
                }

                // Đảo ngược trạng thái
                isLoadState = !isLoadState;
            }
        });



        // Đăng ký sự kiện cho nút chỉ thị
        Button buttonChithi = findViewById(R.id.buttonchithi);
        buttonChithi.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(MainActivity.this, FileListActivity.class);
                startActivity(intent);
            }
        });

        btnLuuTru.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!isStoring) {
                    // Bắt đầu lưu trữ
                    startStoringData();
                    btnLuuTru.setText("Dừng Lưu Trữ");
                    isStoring = true;
                    fileCounter++;
                } else {
                    // Dừng lưu trữ
                    stopStoringData();
                    btnLuuTru.setText("Bắt Đầu Lưu Trữ");
                    isStoring = false;
                }
            }
        });


        // Đăng ký sự kiện cho nút Phòng Chính
        buttonPhongChinh.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Chuyển đến PhongChinhActivity
                Intent intent = new Intent(MainActivity.this, Phongkhachvabep.class);
                startActivity(intent);
            }
        });

        // Đăng ký sự kiện cho nút Phòng Ngủ
        buttonPhongNgu.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Chuyển đến PhongNguActivity
                Intent intent = new Intent(MainActivity.this, PhongNguActivity.class);
                startActivity(intent);
            }
        });

        // Thiết lập sự kiện cho buttonCua
        buttonCua.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                toggleCua(); // Gọi hàm để thay đổi trạng thái cửa
            }
        });

        // Lắng nghe trạng thái cửa từ Firebase
        commandReference.child("balcony_door").addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                if (snapshot.exists()) {
                    isCuaOpen = snapshot.getValue(Integer.class) == 1; // Nhận giá trị Cửa
                    updateImageView(isCuaOpen); // Cập nhật hình ảnh trong ImageView
                    updateButtonCuaText(); // Cập nhật văn bản nút cửa
                }
            }

            @Override
            public void onCancelled(@NonNull DatabaseError error) {
                Log.e("Firebase", "Lỗi khi nhận dữ liệu Cửa: " + error.getMessage());
            }
        });

        // Tải dữ liệu cảm biến
        loadSensorData();
    }

    private void toggleCua() {
        isCuaOpen = !isCuaOpen; // Đảo ngược trạng thái cửa
        commandReference.child("balcony_door").setValue(isCuaOpen ? 1 : 0) // Cập nhật trạng thái cửa lên Firebase
                .addOnCompleteListener(task -> {
                    if (task.isSuccessful()) {
                        Log.d("Firebase", "Trạng thái cửa đã được cập nhật thành công: " + (isCuaOpen ? "Mở" : "Đóng"));
                        updateImageView(isCuaOpen); // Cập nhật hình ảnh trong ImageView
                        updateButtonCuaText(); // Cập nhật văn bản nút cửa
                    } else {
                        Log.e("Firebase", "Lỗi khi cập nhật trạng thái cửa: " + task.getException());
                    }
                });
    }

    private void updateButtonCuaText() {
        // Cập nhật văn bản cho nút cửa
        if (isCuaOpen) {
            buttonCua.setText("Đóng cửa"); // Nếu cửa đang mở, hiển thị "Đóng cửa"
        } else {
            buttonCua.setText("Mở cửa"); // Nếu cửa đang đóng, hiển thị "Mở cửa"
        }
    }

    private void loadSensorData() {
        // Nhận dữ liệu nhiệt độ, độ ẩm và khí gas
        mDatabase.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                if (snapshot.exists()) {
                    for (DataSnapshot childSnapshot : snapshot.getChildren()) {
                        String lastID = childSnapshot.getKey();
                        fetchSensorData(lastID);
                    }
                }
            }

            @Override
            public void onCancelled(@NonNull DatabaseError error) {
                Log.e("Firebase", "Lỗi khi nhận DHTData: " + error.getMessage());
            }
        });
    }

    private void fetchSensorData(String lastID) {
        mDatabase.child(lastID).child("temperature").addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                if (snapshot.exists()) {
                    int temperature = Integer.parseInt(snapshot.getValue(String.class)); // Chuyển đổi giá trị về int
                    if (temperature < 100) {updateTemperature(temperature);}
                }
            }

            @Override
            public void onCancelled(@NonNull DatabaseError error) {
                Log.e("Firebase", "Lỗi khi nhận dữ liệu nhiệt độ: " + error.getMessage());
            }
        });

        mDatabase.child(lastID).child("humidity").addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                if (snapshot.exists()) {
                    int humidity = Integer.parseInt(snapshot.getValue(String.class)); // Chuyển đổi giá trị về int
                    if (humidity < 100) {updateHumidity(humidity);}

                }
            }

            @Override
            public void onCancelled(@NonNull DatabaseError error) {
                Log.e("Firebase", "Lỗi khi nhận dữ liệu độ ẩm: " + error.getMessage());
            }
        });

        mDatabase.child(lastID).child("gas").addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                if (snapshot.exists()) {
                    int gas = Integer.parseInt(snapshot.getValue(String.class)); // Chuyển đổi giá trị về int
                    updateGas(gas);
                }
            }

            @Override
            public void onCancelled(@NonNull DatabaseError error) {
                Log.e("Firebase", "Lỗi khi nhận dữ liệu khí gas: " + error.getMessage());
            }
        });
    }

    private void updateTemperature(int temperature) {
        tvTemperature.setText(temperature + "°C");
        temperatureProgressBar.setProgress(temperature); // Cập nhật ProgressBar cho nhiệt độ
        temperatureEntries.add(new Entry(entryIndex++, temperature)); // Thêm dữ liệu vào danh sách
        updateLineChart(); // Cập nhật biểu đồ
    }

    private void updateHumidity(int humidity) {
        tvHumidity.setText(humidity + "%");
        humidityProgressBar.setProgress(humidity); // Cập nhật ProgressBar cho độ ẩm
        humidityEntries.add(new Entry(entryIndex++, humidity)); // Thêm dữ liệu vào danh sách
        updateLineChart(); // Cập nhật biểu đồ
    }

    private void updateGas(int gas) {
        tvGas.setText(gas + " %");
        gasProgressBar.setProgress(gas); // Cập nhật ProgressBar cho khí gas
        gasEntries.add(new Entry(entryIndex++, gas)); // Thêm dữ liệu vào danh sách
        updateLineChart(); // Cập nhật biểu đồ
    }

    private void updateLineChart() {
        LineDataSet temperatureDataSet = new LineDataSet(temperatureEntries, "Nhiệt độ");
        temperatureDataSet.setColor(ContextCompat.getColor(this, R.color.temperature_color)); // Màu cho nhiệt độ
        temperatureDataSet.setValueTextColor(ContextCompat.getColor(this, R.color.temperature_color)); // Màu chữ cho giá trị nhiệt độ

        LineDataSet humidityDataSet = new LineDataSet(humidityEntries, "Độ ẩm");
        humidityDataSet.setColor(ContextCompat.getColor(this, R.color.humidity_color)); // Màu cho độ ẩm
        humidityDataSet.setValueTextColor(ContextCompat.getColor(this, R.color.humidity_color)); // Màu chữ cho giá trị độ ẩm

        LineDataSet gasDataSet = new LineDataSet(gasEntries, "Khí gas");
        gasDataSet.setColor(ContextCompat.getColor(this, R.color.gas_color)); // Màu cho khí gas
        gasDataSet.setValueTextColor(ContextCompat.getColor(this, R.color.gas_color)); // Màu chữ cho giá trị khí gas

        LineData lineData = new LineData(temperatureDataSet, humidityDataSet, gasDataSet);
        lineChart.setData(lineData);
        lineChart.invalidate(); // Cập nhật lại biểu đồ
    }

    private void updateImageView(boolean isOpen) {
        if (isOpen) {
            imageViewKhoacua.setImageResource(R.drawable.khoamo); // Hình ảnh cửa mở
        } else {
            imageViewKhoacua.setImageResource(R.drawable.khoadong); // Hình ảnh cửa đóng
        }
    }

    // Hàm để reset lại dữ liệu trên LineChart
    private void resetLineChart() {
        // Xóa dữ liệu trong các danh sách
        temperatureEntries.clear();
        humidityEntries.clear();
        gasEntries.clear();
        entryIndex = 0; // Reset chỉ số cho các mục mới

        // Cập nhật lại biểu đồ
        LineData lineData = new LineData();
        lineChart.setData(lineData);
        lineChart.invalidate(); // Cập nhật biểu đồ
    }

    private void startStoringData() {
        String currentTime = java.text.DateFormat.getDateTimeInstance().format(new java.util.Date()); // Lấy thời gian hiện tại

        // Tạo tên tệp mới
        fileName = "data_" + fileCounter + "_" + currentTime + ".txt"; // Tên tệp với số thứ tự và thời gian

        try {
            File file = new File(getExternalFilesDir(null), fileName);
            fos = new FileOutputStream(file, true); // Mở tệp để ghi (chế độ append)
            Toast.makeText(this, "Bắt đầu lưu dữ liệu vào " + fileName, Toast.LENGTH_SHORT).show();

            isStoring = true; // Đặt trạng thái lưu trữ là true

            // Khởi tạo listener để lắng nghe sự thay đổi trong dữ liệu
            mDatabase = FirebaseDatabase.getInstance().getReference("DHTData");
            mDatabase.addValueEventListener(new ValueEventListener() {
                @Override
                public void onDataChange(DataSnapshot dataSnapshot) {
                    if (isStoring) {
                        updateDataInFile(dataSnapshot); // Ghi dữ liệu vào tệp
                    }
                }

                @Override
                public void onCancelled(DatabaseError databaseError) {
                    Toast.makeText(MainActivity.this, "Lỗi khi lắng nghe dữ liệu: " + databaseError.getMessage(), Toast.LENGTH_SHORT).show();
                }
            });

        } catch (IOException e) {
            Toast.makeText(this, "Lỗi khi mở tệp: " + e.getMessage(), Toast.LENGTH_SHORT).show();
        }
    }

    private void updateDataInFile(DataSnapshot dataSnapshot) {
        String currentTime = java.text.DateFormat.getDateTimeInstance().format(new java.util.Date()); // Lấy thời gian hiện tại

        // Lấy giá trị từ DataSnapshot
        String temperature = dataSnapshot.child("temperature").getValue(String.class);
        String humidity = dataSnapshot.child("humidity").getValue(String.class);
        String gas = dataSnapshot.child("gas").getValue(String.class);

        // Lấy dữ liệu từ TextView
        String tvTemperatureValue = tvTemperature.getText().toString();
        String tvHumidityValue = tvHumidity.getText().toString();
        String tvGasValue = tvGas.getText().toString();

        // Tạo chuỗi dữ liệu để lưu
        String dataToSave = "Thời gian: " + currentTime + "\n" +
                "Temperature: " + (temperature != null ? temperature : tvTemperatureValue) +
                "\nHumidity: " + (humidity != null ? humidity : tvHumidityValue) +
                "\nGas: " + (gas != null ? gas : tvGasValue) + "\n\n";

        try {
            if (fos != null) {
                fos.write(dataToSave.getBytes());
                fos.flush(); // Đảm bảo dữ liệu được ghi ra ngay lập tức
            }
        } catch (IOException e) {
            Toast.makeText(this, "Lỗi khi ghi dữ liệu vào tệp: " + e.getMessage(), Toast.LENGTH_SHORT).show();
        }
    }


    private void stopStoringData() {
        if (fos != null) {
            try {
                fos.close(); // Đóng tệp
                fos = null; // Đặt lại biến
                isStoring = false; // Đặt trạng thái lưu trữ là false
            } catch (IOException e) {
                Toast.makeText(this, "Lỗi khi đóng tệp: " + e.getMessage(), Toast.LENGTH_SHORT).show();
            }
        }
    }
}



