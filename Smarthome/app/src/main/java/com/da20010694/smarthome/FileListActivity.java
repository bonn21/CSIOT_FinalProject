package com.da20010694.smarthome;

import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import java.io.File;

public class FileListActivity extends AppCompatActivity {

    private ListView listView;
    private String[] fileList;
    private ArrayAdapter<String> adapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_file_list);

        listView = findViewById(R.id.listViewFiles);
        loadFileList();

        // Thiết lập sự kiện cho ListView
        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                String selectedFile = fileList[position];
                Intent intent = new Intent(FileListActivity.this, FileDataActivity.class);
                intent.putExtra("fileName", selectedFile); // Chuyển tên tệp sang Activity kế tiếp
                startActivity(intent);
            }
        });

        // Thiết lập sự kiện nhấn giữ để xóa tệp
        listView.setOnItemLongClickListener(new AdapterView.OnItemLongClickListener() {
            @Override
            public boolean onItemLongClick(AdapterView<?> parent, View view, int position, long id) {
                confirmDelete(position);
                return true; // Trả về true để đánh dấu sự kiện đã được xử lý
            }
        });
    }

    private void loadFileList() {
        File directory = getExternalFilesDir(null); // Thư mục lưu trữ
        if (directory != null) {
            fileList = directory.list(); // Lấy danh sách tệp
            if (fileList != null) {
                adapter = new ArrayAdapter<>(this, android.R.layout.simple_list_item_1, fileList);
                listView.setAdapter(adapter); // Gán adapter cho ListView
            } else {
                Toast.makeText(this, "Không có tệp nào!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Log.e("FileListActivity", "Không tìm thấy thư mục lưu trữ!");
        }
    }

    private void confirmDelete(int position) {
        new AlertDialog.Builder(this)
                .setTitle("Xác nhận xóa")
                .setMessage("Bạn có chắc chắn muốn xóa tệp: " + fileList[position] + " không?")
                .setPositiveButton("Có", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        deleteFile(position);
                    }
                })
                .setNegativeButton("Không", null)
                .show();
    }

    private void deleteFile(int position) {
        File directory = getExternalFilesDir(null);
        if (directory != null) {
            File fileToDelete = new File(directory, fileList[position]);
            if (fileToDelete.delete()) {
                Toast.makeText(this, "Đã xóa tệp: " + fileList[position], Toast.LENGTH_SHORT).show();
                // Cập nhật danh sách
                loadFileList();
            } else {
                Toast.makeText(this, "Không thể xóa tệp: " + fileList[position], Toast.LENGTH_SHORT).show();
            }
        }
    }
}
