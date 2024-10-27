package com.da20010694.smarthome;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

public class MyReceiver extends BroadcastReceiver {
    @Override
    public void onReceive(Context context, Intent intent) {
        if ("STOP_ALERT".equals(intent.getAction())) {
            // Dừng dịch vụ cảnh báo
            context.stopService(new Intent(context, GasTempAlertService.class));
        }
    }
}
