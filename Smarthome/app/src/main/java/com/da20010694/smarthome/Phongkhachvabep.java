package com.da20010694.smarthome;

import android.animation.ObjectAnimator;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

import androidx.appcompat.app.AppCompatActivity;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

public class Phongkhachvabep extends AppCompatActivity {

    // Khai báo các biến để kết nối Firebase
    private DatabaseReference databaseReference;
    private boolean isLightOn = false; // Trạng thái đèn
    private boolean isLivingFanOn = false; // Trạng thái quạt phòng khách
    private boolean isKitchenFanOn = false; // Trạng thái quạt khu bếp
    private ImageView den1; // Khai báo ImageView cho đèn
    private ImageView buttonLivingFan; // Khai báo ImageView cho quạt phòng khách
    private ImageView buttonKitchenFan; // Khai báo ImageView cho quạt khu bếp
    private ObjectAnimator animatorLivingFan; // Animator cho quạt phòng khách
    private ObjectAnimator animatorKitchenFan; // Animator cho quạt khu bếp

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.phongchinh); // Đảm bảo rằng bạn có layout này

        // Khởi tạo Firebase Database
        databaseReference = FirebaseDatabase.getInstance().getReference("command");

        // Khởi tạo ImageView cho đèn và quạt
        den1 = findViewById(R.id.den1);
        buttonLivingFan = findViewById(R.id.buttonquat1);
        buttonKitchenFan = findViewById(R.id.buttonquat2);

        // Khởi tạo Animator cho quạt
        animatorLivingFan = ObjectAnimator.ofFloat(buttonLivingFan, "rotation", 0f, 360f); // Quạt phòng khách
        animatorLivingFan.setDuration(500); // Thời gian quay một vòng
        animatorLivingFan.setRepeatCount(ObjectAnimator.INFINITE); // Lặp vô hạn

        animatorKitchenFan = ObjectAnimator.ofFloat(buttonKitchenFan, "rotation", 0f, 360f); // Quạt khu bếp
        animatorKitchenFan.setDuration(500); // Thời gian quay một vòng
        animatorKitchenFan.setRepeatCount(ObjectAnimator.INFINITE); // Lặp vô hạn

        // Khởi tạo nút "Trở về"
        Button trove1 = findViewById(R.id.trove1);
        trove1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Quay lại MainActivity
                Intent intent = new Intent(Phongkhachvabep.this, MainActivity.class);
                startActivity(intent);
                finish(); // Kết thúc hoạt động hiện tại nếu bạn muốn không quay lại
            }
        });

        // Khởi tạo button "Bật đèn"
        Button batden1 = findViewById(R.id.batden1);
        batden1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                isLightOn = !isLightOn; // Đảo trạng thái
                databaseReference.child("living_led").setValue(isLightOn ? 1 : 0); // Cập nhật Firebase
            }
        });

        // Khởi tạo button "Bật quạt phòng khách"
        Button batquat1 = findViewById(R.id.batquat1);
        batquat1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                isLivingFanOn = !isLivingFanOn; // Đảo trạng thái
                databaseReference.child("living_fan").setValue(isLivingFanOn ? 1 : 0); // Cập nhật Firebase
                if (isLivingFanOn) {
                    animatorLivingFan.start(); // Bắt đầu quay quạt
                } else {
                    animatorLivingFan.end(); // Dừng quay quạt
                    buttonLivingFan.setRotation(0); // Đặt lại vị trí quay
                }
            }
        });

        // Khởi tạo button "Bật quạt khu bếp"
        Button batquat2 = findViewById(R.id.batquat2);
        batquat2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                isKitchenFanOn = !isKitchenFanOn; // Đảo trạng thái
                databaseReference.child("kitchen_fan").setValue(isKitchenFanOn ? 1 : 0); // Cập nhật Firebase
                if (isKitchenFanOn) {
                    animatorKitchenFan.start(); // Bắt đầu quay quạt
                } else {
                    animatorKitchenFan.end(); // Dừng quay quạt
                    buttonKitchenFan.setRotation(0); // Đặt lại vị trí quay
                }
            }
        });

        // Lắng nghe sự thay đổi dữ liệu từ Firebase cho đèn
        databaseReference.child("living_led").addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot snapshot) {
                isLightOn = snapshot.getValue(Integer.class) == 1; // Chuyển đổi giá trị về trạng thái boolean
                batden1.setText(isLightOn ? "Tắt đèn" : "Bật đèn"); // Cập nhật văn bản

                // Cập nhật hình ảnh cho ImageView dựa trên trạng thái đèn
                if (isLightOn) {
                    den1.setImageResource(R.drawable.circle_light_on); // Đèn bật
                } else {
                    den1.setImageResource(R.drawable.circle_light_off); // Đèn tắt
                }
            }

            @Override
            public void onCancelled(DatabaseError error) {
                // Xử lý lỗi nếu cần
            }
        });

        // Lắng nghe sự thay đổi dữ liệu từ Firebase cho quạt phòng khách
        databaseReference.child("living_fan").addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot snapshot) {
                isLivingFanOn = snapshot.getValue(Integer.class) == 1; // Chuyển đổi giá trị về trạng thái boolean
                batquat1.setText(isLivingFanOn ? "Tắt quạt phòng khách" : "Bật quạt phòng khách"); // Cập nhật văn bản

                // Kiểm tra trạng thái quạt và bắt đầu hoặc dừng quay quạt
                if (isLivingFanOn) {
                    animatorLivingFan.start(); // Bắt đầu quay quạt
                } else {
                    animatorLivingFan.end(); // Dừng quay quạt
                    buttonLivingFan.setRotation(0); // Đặt lại vị trí quay
                }
            }

            @Override
            public void onCancelled(DatabaseError error) {
                // Xử lý lỗi nếu cần
            }
        });

        // Lắng nghe sự thay đổi dữ liệu từ Firebase cho quạt khu bếp
        databaseReference.child("kitchen_fan").addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot snapshot) {
                isKitchenFanOn = snapshot.getValue(Integer.class) == 1; // Chuyển đổi giá trị về trạng thái boolean
                batquat2.setText(isKitchenFanOn ? "Tắt quạt khu bếp" : "Bật quạt khu bếp"); // Cập nhật văn bản

                // Kiểm tra trạng thái quạt và bắt đầu hoặc dừng quay quạt
                if (isKitchenFanOn) {
                    animatorKitchenFan.start(); // Bắt đầu quay quạt
                } else {
                    animatorKitchenFan.end(); // Dừng quay quạt
                    buttonKitchenFan.setRotation(0); // Đặt lại vị trí quay
                }
            }

            @Override
            public void onCancelled(DatabaseError error) {
                // Xử lý lỗi nếu cần
            }
        });
    }
}
