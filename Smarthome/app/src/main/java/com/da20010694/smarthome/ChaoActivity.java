package com.da20010694.smarthome;

import android.animation.ObjectAnimator;
import android.content.Intent;
import android.os.Bundle;
import android.widget.ImageView;
import android.widget.ProgressBar;

import androidx.appcompat.app.AppCompatActivity;

public class ChaoActivity extends AppCompatActivity {

    private ImageView imageView11;
    private ImageView chuchaoImageView;
    private ProgressBar progressBar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.logochao); // Sử dụng layout cho màn hình chào

        // Tham chiếu tới các phần tử
        imageView11 = findViewById(R.id.imageView11);
        chuchaoImageView = findViewById(R.id.chuchao);
        progressBar = findViewById(R.id.progressBar);

        // Bắt đầu animation
        startAnimation();
    }

    private void startAnimation() {
        // Hiệu ứng cho imageView11 xuất hiện
        ObjectAnimator alphaAnimator1 = ObjectAnimator.ofFloat(imageView11, "alpha", 0f, 1f);
        alphaAnimator1.setDuration(3000); // Thời gian xuất hiện (3 giây)
        alphaAnimator1.start();

        // Sau khi imageView11 xuất hiện xong, mới cho chuchao và progressBar xuất hiện
        alphaAnimator1.addListener(new android.animation.AnimatorListenerAdapter() {
            @Override
            public void onAnimationEnd(android.animation.Animator animation) {
                // Hiệu ứng cho chuchao và progressBar xuất hiện
                ObjectAnimator alphaAnimator2 = ObjectAnimator.ofFloat(chuchaoImageView, "alpha", 0f, 1f);
                ObjectAnimator alphaAnimator3 = ObjectAnimator.ofFloat(progressBar, "alpha", 0f, 1f);

                alphaAnimator2.setDuration(3000); // Thời gian xuất hiện (3 giây)
                alphaAnimator3.setDuration(3000); // Thời gian xuất hiện (3 giây)

                alphaAnimator2.start();
                alphaAnimator3.start();

                // Cập nhật ProgressBar
                updateProgressBar();
            }
        });
    }

    private void updateProgressBar() {
        // Cập nhật ProgressBar từ 0 đến 100 trong 3 giây
        new Thread(() -> {
            for (int progress = 0; progress <= 100; progress++) {
                // Cập nhật ProgressBar trên UI thread
                int finalProgress = progress;
                runOnUiThread(() -> progressBar.setProgress(finalProgress));

                // Dừng 30 ms trước khi tiếp tục
                try {
                    Thread.sleep(30);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

            // Chuyển đến MainActivity khi ProgressBar hoàn tất
            runOnUiThread(() -> {
                Intent intent = new Intent(ChaoActivity.this, MainActivity.class);
                startActivity(intent);
                finish(); // Đóng ChaoActivity
            });
        }).start();
    }
}
