package com.da20010694.smarthome;

import android.annotation.SuppressLint;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.media.MediaPlayer;
import android.os.Build;
import android.os.Handler;
import android.os.IBinder;
import android.os.Vibrator;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.app.NotificationCompat;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

public class GasTempAlertService extends Service {
    private DatabaseReference cDatabase;
    private MediaPlayer mediaPlayer;
    private Vibrator vibrator;
    private NotificationManager notificationManager;
    private static final int NOTIFICATION_ID = 1;
    private Handler handler = new Handler();
    private Runnable vibrationRunnable;

    @Override
    public void onCreate() {
        super.onCreate();
        cDatabase = FirebaseDatabase.getInstance().getReference("command/garden_buzzer");
        notificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);

        // Khởi tạo MediaPlayer với chuông cảnh báo
        mediaPlayer = MediaPlayer.create(this, R.raw.alarm_sound); // Thay 'alarm_sound' bằng file chuông của bạn
        mediaPlayer.setLooping(true); // Phát chuông cảnh báo liên tục

        // Khởi tạo Vibrator
        vibrator = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);

        // Tạo kênh thông báo cho Foreground Service
        createNotificationChannel();

        // Bắt đầu dịch vụ foreground
        startForegroundService();

        // Lắng nghe thay đổi của garden_buzzer
        cDatabase.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                if (snapshot.exists() && snapshot.getValue(Integer.class) == 1) {
                    triggerAlert("Cảnh báo nhà của bạn đang gặp sự cố hãy kiểm tra!");
                } else if (snapshot.exists() && snapshot.getValue(Integer.class) == 0) {
                    stopAlert(); // Tắt chuông và rung khi garden_buzzer = 0
                }
            }

            @Override
            public void onCancelled(@NonNull DatabaseError error) {
                Log.e("Firebase", "Lỗi khi nhận dữ liệu garden_buzzer: " + error.getMessage());
            }
        });
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    private void createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel channel = new NotificationChannel("alert_channel", "Alerts", NotificationManager.IMPORTANCE_HIGH);
            notificationManager.createNotificationChannel(channel);
        }
    }

    private void startForegroundService() {
        // Tạo Intent để mở ứng dụng khi nhấn vào thông báo
        Intent intent = new Intent(this, MainActivity.class);
        PendingIntent pendingIntent = PendingIntent.getActivity(this, 0, intent, PendingIntent.FLAG_UPDATE_CURRENT);

        // Tạo thông báo
        NotificationCompat.Builder builder = new NotificationCompat.Builder(this, "alert_channel")
                .setSmallIcon(R.mipmap.ic_launcher) // Icon của thông báo
                .setContentTitle("Cảnh báo")
                .setContentText("Dịch vụ cảnh báo đang chạy...")
                .setPriority(NotificationCompat.PRIORITY_HIGH)
                .setContentIntent(pendingIntent)
                .setAutoCancel(false); // Không tự động tắt thông báo

        startForeground(NOTIFICATION_ID, builder.build()); // Bắt đầu dịch vụ foreground với thông báo
    }

    private void triggerAlert(String message) {
        if (!mediaPlayer.isPlaying()) {
            mediaPlayer.start(); // Phát chuông cảnh báo
        }

        // Cài đặt rung liên tục mỗi 10 giây
        vibrationRunnable = new Runnable() {
            @Override
            public void run() {
                if (vibrator != null) {
                    vibrator.vibrate(10000); // Rung trong 10 giây
                }
                handler.postDelayed(this, 10000); // Lặp lại mỗi 10 giây
            }
        };
        handler.post(vibrationRunnable);

        // Hiển thị thông báo
        showNotification(message);
        Log.d("Alert", message); // Hiển thị thông báo log
    }

    @SuppressLint("NotificationPermission")
    private void showNotification(String message) {
        // Tạo Intent để tắt chuông và rung
        Intent stopIntent = new Intent(this, MyReceiver.class);
        stopIntent.setAction("STOP_ALERT");
        PendingIntent stopPendingIntent = PendingIntent.getBroadcast(this, 0, stopIntent, PendingIntent.FLAG_UPDATE_CURRENT);

        // Tạo thông báo
        NotificationCompat.Builder builder = new NotificationCompat.Builder(this, "alert_channel")
                .setSmallIcon(R.mipmap.ic_launcher) // Icon của thông báo
                .setContentTitle("Cảnh báo")
                .setContentText(message + ". có thể chọn 'Tắt' để tắt cảnh báo.")
                .setPriority(NotificationCompat.PRIORITY_HIGH)
                .addAction(R.drawable.stop, "Tắt", stopPendingIntent) // Thêm nút "Tắt" vào thông báo
                .setOngoing(true); // Đánh dấu thông báo là ongoing

        notificationManager.notify(NOTIFICATION_ID, builder.build()); // Hiển thị thông báo
    }

    public void stopAlert() {
        if (mediaPlayer.isPlaying()) {
            mediaPlayer.stop(); // Dừng chuông cảnh báo
            mediaPlayer.reset(); // Reset MediaPlayer để có thể phát lại
            mediaPlayer.release(); // Giải phóng tài nguyên
        }

        // Hủy rung liên tục
        handler.removeCallbacks(vibrationRunnable);
        if (vibrator != null) {
            vibrator.cancel(); // Tắt rung
        }

        // Cập nhật giá trị garden_buzzer trên Firebase thành 0 để tắt còi
        cDatabase.setValue(0).addOnCompleteListener(task -> {
            if (task.isSuccessful()) {
                // Hủy thông báo khi cảnh báo đã tắt
                notificationManager.cancel(NOTIFICATION_ID);
                Log.d("Alert", "Cảnh báo đã được tắt");
            } else {
                Log.e("Firebase", "Không thể cập nhật giá trị garden_buzzer");
            }
        });
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        stopAlert(); // Dừng chuông và rung khi dịch vụ bị dừng
    }
}
