package com.da20010694.smarthome;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

public class PhongNguActivity extends AppCompatActivity {

    private ImageView denNguImageView;
    private Button batDenButton;
    private Button troveButton; // Nút trở về
    private boolean isDenNguOn = false; // Trạng thái đèn
    private DatabaseReference databaseReference;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.phongngu); // Sử dụng layout cho phòng ngủ

        // Tham chiếu tới các thành phần giao diện
        denNguImageView = findViewById(R.id.denngu);
        batDenButton = findViewById(R.id.batdenngu);
        troveButton = findViewById(R.id.trove2); // Tham chiếu nút "Trở về"

        // Tham chiếu tới Firebase với khóa mới
        databaseReference = FirebaseDatabase.getInstance().getReference("command").child("bedroom_led");

        // Lấy trạng thái đèn từ Firebase
        databaseReference.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                if (snapshot.exists()) {
                    isDenNguOn = snapshot.getValue(Integer.class) == 1; // Kiểm tra trạng thái
                    updateDenNguUI(); // Cập nhật giao diện
                }
            }

            @Override
            public void onCancelled(@NonNull DatabaseError error) {
                // Xử lý lỗi và thông báo cho người dùng
                denNguImageView.setImageResource(R.drawable.circle_light_off); // Set default image on error
                batDenButton.setEnabled(false); // Disable button until we can fetch data
            }
        });

        // Xử lý sự kiện khi nhấn nút bật đèn
        batDenButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                toggleDenNgu(); // Chuyển đổi trạng thái đèn
            }
        });

        // Xử lý sự kiện khi nhấn nút trở về
        troveButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Trở về MainActivity
                Intent intent = new Intent(PhongNguActivity.this, MainActivity.class);
                startActivity(intent);
                finish(); // Đóng activity hiện tại
            }
        });
    }

    // Hàm để bật/tắt đèn và cập nhật Firebase
    private void toggleDenNgu() {
        isDenNguOn = !isDenNguOn; // Chuyển đổi trạng thái
        databaseReference.setValue(isDenNguOn ? 1 : 0); // Cập nhật trạng thái lên Firebase
        updateDenNguUI(); // Cập nhật giao diện
    }

    // Cập nhật giao diện hình ảnh đèn và nút
    private void updateDenNguUI() {
        if (isDenNguOn) {
            denNguImageView.setImageResource(R.drawable.circle_light_on); // Hình ảnh đèn sáng
            batDenButton.setText("Tắt đèn"); // Cập nhật nút
        } else {
            denNguImageView.setImageResource(R.drawable.circle_light_off); // Hình ảnh đèn tắt
            batDenButton.setText("Bật đèn"); // Cập nhật nút
        }
    }
}
